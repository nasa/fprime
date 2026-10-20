#!/usr/bin/env node
// External-CI orchestrator (dependency-free Node >= 20).
//
//   node ext-ci-orchestrator.mjs run       dispatch, poll, report
//   node ext-ci-orchestrator.mjs finalize  close anything still open (runs under if: always())
//
// Two credentials, never mixed:
//   GITHUB_TOKEN   this repo only (checks/pull-requests/issues write) - reports results
//   DISPATCH_TOKEN external repos only (actions: write)                - starts remote runs
import fs from 'node:fs';

const env = process.env;
const mode = process.argv[2] ?? 'run';
const API = env.GITHUB_API_URL || 'https://api.github.com';
const SERVER = env.GITHUB_SERVER_URL || 'https://github.com';
const STATE_FILE = 'ext-ci-state.json';
const RESULTS_FILE = 'ext-ci-results.json';
const MANIFEST_PATH = '.github/ext-ci-manifest.json';
const POLL_MS = Number(env.POLL_INTERVAL_SECONDS || 60) * 1000;
const CORRELATE_TIMEOUT_MS = 5 * 60 * 1000;
const BOT_LOGIN = 'github-actions[bot]';
const NOTIFY_LABEL = 'ext-ci';

const repo = env.GITHUB_REPOSITORY;
const runId = env.GITHUB_RUN_ID;
const attempt = Number(env.GITHUB_RUN_ATTEMPT || 1);
const eventName = env.GITHUB_EVENT_NAME;
const event = JSON.parse(fs.readFileSync(env.GITHUB_EVENT_PATH, 'utf8'));
const orchestratorUrl = `${SERVER}/${repo}/actions/runs/${runId}${attempt > 1 ? `/attempts/${attempt}` : ''}`;

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const short = (sha) => sha.slice(0, 7);
// Remote-controlled text (job/step names, branch names) is only ever rendered inside code spans.
const code = (s) => '`' + String(s).replace(/[`\r\n]/g, ' ') + '`';

// ---------------------------------------------------------------- REST helpers
class ApiError extends Error { constructor(msg, status) { super(msg); this.status = status; } }

function client(token, label) {
  const etags = new Map();
  async function request(method, url, body, { conditional = false } = {}) {
    if (!token) throw new ApiError(`${label} token missing (secret not configured?)`, 0);
    const full = url.startsWith('http') ? url : API + url;
    const headers = {
      authorization: `Bearer ${token}`,
      accept: 'application/vnd.github+json',
      'x-github-api-version': '2022-11-28',
      'user-agent': 'fprime-ext-ci-orchestrator',
      ...(body ? { 'content-type': 'application/json' } : {}),
    };
    if (conditional && etags.has(full)) headers['if-none-match'] = etags.get(full).etag;
    for (let i = 0; ; i++) {
      let res;
      try {
        res = await fetch(full, { method, headers, body: body ? JSON.stringify(body) : undefined });
      } catch (e) {
        if (i >= 4) throw new ApiError(`${label}: ${method} ${url} -> network error ${e.message}`, 0);
        await sleep(2000 * 2 ** i); continue;
      }
      if (res.status === 304 && conditional) return { status: 200, ok: true, data: etags.get(full).data, cached: true };
      const text = await res.text();
      let data = null;
      try { data = text ? JSON.parse(text) : null; } catch { data = text; }
      const remaining = Number(res.headers.get('x-ratelimit-remaining'));
      const limited = res.status === 429 || (res.status === 403 && (remaining === 0 || /rate limit/i.test(text)));
      if ((limited || res.status >= 500) && i < 4) {
        let wait = 2000 * 2 ** i;
        const ra = Number(res.headers.get('retry-after'));
        const reset = Number(res.headers.get('x-ratelimit-reset'));
        if (ra) wait = ra * 1000;
        else if (limited && reset) wait = Math.max(wait, reset * 1000 - Date.now() + 1000);
        console.log(`::warning::${label}: ${method} ${url} -> ${res.status}; retrying in ${Math.round(wait / 1000)} s`);
        await sleep(Math.min(wait, 15 * 60_000)); continue;
      }
      if (res.ok && conditional && res.headers.get('etag')) etags.set(full, { etag: res.headers.get('etag'), data });
      return { status: res.status, ok: res.ok, data };
    }
  }
  async function api(method, url, body, opts) {
    const r = await request(method, url, body, opts);
    if (!r.ok) throw new ApiError(`${label}: ${method} ${url} -> ${r.status} ${JSON.stringify(r.data).slice(0, 300)}`, r.status);
    return r.data;
  }
  return {
    get: (u, opts) => api('GET', u, undefined, opts),
    post: (u, b) => api('POST', u, b),
    patch: (u, b) => api('PATCH', u, b),
    // true/false for 200/404; anything else is an error, never silently "absent".
    exists: async (u) => {
      const r = await request('GET', u);
      if (r.status === 200) return true;
      if (r.status === 404) return false;
      throw new ApiError(`${label}: GET ${u} -> ${r.status}`, r.status);
    },
  };
}
const own = client(env.GITHUB_TOKEN, 'GITHUB_TOKEN');
const ext = client(env.DISPATCH_TOKEN, 'DISPATCH_TOKEN');

// ---------------------------------------------------------------- target resolution
// What F´ revision are we testing, and which PR (if any) gets the Check Runs?
async function resolveTarget() {
  const t = { pr: null, headRepo: repo, headSha: null, headBranch: null, baseRef: null, beforeSha: null };
  if (eventName === 'workflow_run') {
    const wr = event.workflow_run;
    // Trust only the event payload: nothing produced by the (untrusted) trigger run is read.
    t.headSha = wr.head_sha;
    t.headRepo = wr.head_repository.full_name;
    t.headBranch = wr.head_branch;
    if (wr.event === 'merge_group') {
      // Merge-queue branch gh-readonly-queue/<base>/pr-<N>-<sha>: checks go on the queue SHA, no PR.
      const m = /^gh-readonly-queue\/(.+?)\/pr-\d+-/.exec(t.headBranch || '');
      t.baseRef = m ? m[1] : env.GITHUB_REF_NAME;
      return t;
    }
    // workflow_run.pull_requests is empty for fork PRs; find the PR by commit, then by head ref.
    let pr = (await own.get(`/repos/${repo}/commits/${t.headSha}/pulls?per_page=20`))
      .find((p) => p.state === 'open' && p.head.sha === t.headSha && p.base.repo.full_name === repo);
    if (!pr) {
      const owner = t.headRepo.split('/')[0];
      pr = (await own.get(`/repos/${repo}/pulls?state=open&head=${encodeURIComponent(`${owner}:${t.headBranch}`)}`))
        .find((p) => p.head.sha === t.headSha);
    }
    if (!pr) throw new Error(`no open PR found for ${t.headRepo}@${t.headSha} (${t.headBranch}); the PR may have been updated or closed`);
    t.pr = pr.number;
    t.baseRef = pr.base.ref;
  } else if (eventName === 'workflow_dispatch') {
    const prNumber = (env.INPUT_PR_NUMBER || '').trim();
    if (prNumber) {
      if (!/^\d+$/.test(prNumber)) throw new Error(`pr_number must be numeric, got ${JSON.stringify(prNumber)}`);
      const pr = await own.get(`/repos/${repo}/pulls/${prNumber}`);
      t.pr = pr.number; t.headSha = pr.head.sha; t.headRepo = pr.head.repo.full_name;
      t.headBranch = pr.head.ref; t.baseRef = pr.base.ref;
    } else {
      const ref = (env.INPUT_FPRIME_REF || '').trim() || env.GITHUB_REF_NAME;
      const commit = await own.get(`/repos/${repo}/commits/${encodeURIComponent(ref)}`);
      t.headSha = commit.sha; t.headBranch = ref; t.baseRef = ref;
    }
  } else { // push, schedule
    t.headSha = env.GITHUB_SHA; t.headBranch = env.GITHUB_REF_NAME; t.baseRef = env.GITHUB_REF_NAME;
    if (eventName === 'push' && event.before && !/^0+$/.test(event.before)) t.beforeSha = event.before;
  }
  return t;
}

// Manifest is read from the base branch of the PR (the orchestrator itself always runs from
// the default branch), so release/** PRs use their own branch's manifest. Only a missing file
// falls back to the checked-out copy; any other error is fatal.
async function loadManifest(ref) {
  try {
    const file = await own.get(`/repos/${repo}/contents/${MANIFEST_PATH}?ref=${encodeURIComponent(ref)}`);
    return JSON.parse(Buffer.from(file.content, 'base64').toString('utf8'));
  } catch (e) {
    if (!(e instanceof ApiError && e.status === 404)) throw e;
    console.log(`::warning::manifest missing at ${ref}; using the checked-out copy`);
    return JSON.parse(fs.readFileSync(MANIFEST_PATH, 'utf8'));
  }
}

const checkName = (manifest, id) => `${manifest.check_prefix || 'ext-ci'} / ${id}`;
const ourExternalId = (id) => `${runId}-${id}`; // same across attempts of one run

// ---------------------------------------------------------------- change filter
// paths-ignore lives here, not on the trigger workflow, so that filtered PRs still get a
// (skipped) Check Run per repo and required checks can be satisfied.
function globToRegExp(glob) {
  let re = '^';
  for (let i = 0; i < glob.length; i++) {
    const c = glob[i];
    if (c === '*') {
      if (glob[i + 1] === '*') { re += '.*'; i++; if (glob[i + 1] === '/') i++; }
      else re += '[^/]*';
    } else if (c === '?') re += '[^/]';
    else re += c.replace(/[.+^${}()|[\]\\]/g, '\\$&');
  }
  return new RegExp(re + '$');
}

async function changedFiles(target) {
  let url = null;
  if (target.pr) url = `/repos/${repo}/pulls/${target.pr}/files?per_page=100`;
  else if (target.beforeSha) url = `/repos/${repo}/compare/${target.beforeSha}...${target.headSha}?per_page=100`;
  if (!url) return null; // unknown -> run everything
  const files = [];
  for (let page = 1; page <= 30; page++) {
    const r = await own.get(`${url}&page=${page}`);
    const batch = target.pr ? r : (r.files || []);
    files.push(...batch.map((f) => f.filename));
    if (batch.length < 100) break;
  }
  return files;
}

function relevantChange(files, manifest) {
  const ignore = (manifest.paths_ignore || []).map(globToRegExp);
  if (!files || !ignore.length) return true;
  return files.some((f) => !ignore.some((re) => re.test(f)));
}

// ---------------------------------------------------------------- selection
async function latestOwnCheckRun(name, id, sha) {
  const r = await own.get(`/repos/${repo}/commits/${sha}/check-runs?check_name=${encodeURIComponent(name)}&filter=all&per_page=50`);
  // Only reuse a check this very orchestrator run created (external_id stamped with our run id).
  return r.check_runs.filter((c) => c.external_id === ourExternalId(id) && c.app && c.app.slug === 'github-actions')
    .sort((a, b) => b.id - a.id)[0] ?? null;
}

async function selectRepos(manifest, target) {
  let repos = manifest.repos;
  const wanted = (env.INPUT_REPOS || 'all').trim();
  if (eventName === 'workflow_dispatch' && wanted && wanted !== 'all') {
    const ids = wanted.split(',').map((s) => s.trim()).filter(Boolean);
    const unknown = ids.filter((id) => !repos.some((r) => r.id === id));
    if (unknown.length) throw new Error(`unknown manifest ids: ${unknown.join(', ')}`);
    repos = repos.filter((r) => ids.includes(r.id));
  } else if (target.pr) {
    repos = repos.filter((r) => r.on_pr !== false);
  }
  // Failed-only re-run: on attempt > 1 keep every repo whose Check Run from attempt 1..n-1 of
  // this same run already succeeded, and dispatch only the rest.
  const carried = [];
  const dispatch = [];
  const forceAll = (env.INPUT_FORCE_ALL || '').toString() === 'true';
  for (const entry of repos) {
    const prior = attempt > 1 && !forceAll ? await latestOwnCheckRun(checkName(manifest, entry.id), entry.id, target.headSha) : null;
    if (prior && prior.status === 'completed' && prior.conclusion === 'success') carried.push({ entry, prior });
    else dispatch.push(entry);
  }
  return { carried, dispatch };
}

// pr-<N> branch on the ext repo, else the F´ base branch name, else the manifest default.
async function resolveExtRef(entry, target) {
  const candidates = [];
  if (target.pr) candidates.push(`pr-${target.pr}`);
  if (target.baseRef) candidates.push(target.baseRef);
  for (const c of candidates) {
    if (await ext.exists(`/repos/${entry.repository}/branches/${encodeURIComponent(c)}`)) return c;
  }
  return entry.default_ref || 'main';
}

// ---------------------------------------------------------------- state
function saveState(state) { fs.writeFileSync(STATE_FILE, JSON.stringify(state, null, 2)); }
function loadState() { return fs.existsSync(STATE_FILE) ? JSON.parse(fs.readFileSync(STATE_FILE, 'utf8')) : null; }

// ---------------------------------------------------------------- dispatch
async function dispatchOne(manifest, entry, target, state) {
  const dispatchId = `${runId}-${attempt}-${entry.id}`;
  const name = checkName(manifest, entry.id);
  const s = state.repos[entry.id] = {
    id: entry.id, repository: entry.repository, workflow: entry.workflow, ext_ref: null,
    failure_policy: entry.failure_policy || 'block', owners: entry.owners || [],
    timeout_minutes: entry.timeout_minutes || 60, dispatch_id: dispatchId, dispatched_at: new Date().toISOString(),
    check_run_id: null, remote_run_id: null, remote_run_url: null, remote_started_at: null, remote_conclusion: null, conclusion: null,
  };
  try {
    // Check Run first, so a credential or API problem on the ext side still ends in a visible
    // failed check rather than a required check that never appears.
    const check = await own.post(`/repos/${repo}/check-runs`, {
      name, head_sha: target.headSha, status: 'queued', external_id: ourExternalId(entry.id), details_url: orchestratorUrl,
      output: {
        title: `Dispatching ${entry.repository}`,
        summary: `Dispatching ${code(entry.workflow)} on ${code(entry.repository)} with F´ ${code(`${target.headRepo}@${short(target.headSha)}`)}.\n\nOrchestrator: ${orchestratorUrl}`,
      },
    });
    s.check_run_id = check.id;
    saveState(state);
    s.ext_ref = await resolveExtRef(entry, target);
    const r = await ext.post(`/repos/${entry.repository}/actions/workflows/${entry.workflow}/dispatches`, {
      ref: s.ext_ref,
      inputs: {
        fprime_repository: target.headRepo, fprime_ref: target.headSha,
        fprime_pr_number: target.pr ? String(target.pr) : '', dispatch_id: dispatchId,
      },
      return_run_details: true,
    });
    if (r && r.workflow_run_id) {
      s.remote_run_id = r.workflow_run_id; s.remote_run_url = r.html_url;
      await markRunning(s);
    }
    console.log(`[${entry.id}] dispatched ${entry.repository}/${entry.workflow} @ ${s.ext_ref}${s.remote_run_url ? ` -> ${s.remote_run_url}` : ` (dispatch_id=${dispatchId})`}`);
  } catch (e) {
    s.error = `dispatch failed: ${e.message}`;
    s.remote_conclusion = 'dispatch_failed';
    console.log(`::error::[${entry.id}] ${s.error}`);
  }
  saveState(state);
}

async function markRunning(s) {
  await own.patch(`/repos/${repo}/check-runs/${s.check_run_id}`, {
    status: 'in_progress', details_url: s.remote_run_url,
    output: { title: `Running on ${s.repository} @ ${s.ext_ref}`, summary: `Remote run: ${s.remote_run_url}\n\nOrchestrator: ${orchestratorUrl}` },
  });
}

// Fallback only for API servers that still answer 204 to the dispatch call: find the run whose
// title carries our dispatch_id (the ext workflow echoes it into run-name).
async function correlate(state) {
  const pending = Object.values(state.repos).filter((s) => !s.remote_run_id && !s.remote_conclusion);
  const deadline = Date.now() + CORRELATE_TIMEOUT_MS;
  while (pending.length && Date.now() < deadline) {
    for (const s of [...pending]) {
      try {
        const since = new Date(new Date(s.dispatched_at).getTime() - 60_000).toISOString();
        const runs = await ext.get(`/repos/${s.repository}/actions/runs?event=workflow_dispatch&created=${encodeURIComponent('>=' + since)}&per_page=50`);
        const run = runs.workflow_runs.find((r) => (r.display_title || r.name || '').split(/\s+/).includes(s.dispatch_id));
        if (!run) continue;
        s.remote_run_id = run.id; s.remote_run_url = run.html_url;
        pending.splice(pending.indexOf(s), 1);
        console.log(`[${s.id}] remote run ${run.html_url}`);
        await markRunning(s);
        saveState(state);
      } catch (e) { console.log(`::warning::[${s.id}] correlate: ${e.message}`); }
    }
    if (pending.length) await sleep(15_000);
  }
  for (const s of pending) {
    s.remote_conclusion = 'not_found';
    s.error = `no run with dispatch_id ${s.dispatch_id} appeared on ${s.repository} within ${CORRELATE_TIMEOUT_MS / 60000} min`;
    console.log(`::error::[${s.id}] ${s.error}`);
  }
  saveState(state);
}

// ---------------------------------------------------------------- poll
async function pollAll(state) {
  const active = () => Object.values(state.repos).filter((s) => s.remote_run_id && !s.remote_conclusion);
  while (active().length) {
    await sleep(POLL_MS);
    for (const s of active()) {
      try {
        const run = await ext.get(`/repos/${s.repository}/actions/runs/${s.remote_run_id}`, { conditional: true });
        if (run.run_started_at && !s.remote_started_at) s.remote_started_at = run.run_started_at;
        if (run.status === 'completed') {
          s.remote_conclusion = run.conclusion;
          console.log(`[${s.id}] completed: ${run.conclusion}`);
        } else if (s.remote_started_at && Date.now() - new Date(s.remote_started_at).getTime() > s.timeout_minutes * 60_000) {
          // Timeout counts from when the remote run started, not from dispatch (queue time is not the ext repo's fault).
          console.log(`::warning::[${s.id}] timed out after ${s.timeout_minutes} min of running; cancelling remote run`);
          await ext.post(`/repos/${s.repository}/actions/runs/${s.remote_run_id}/cancel`).catch(() => {});
          s.remote_conclusion = 'timed_out';
        } else if (!s.remote_started_at && Date.now() - new Date(s.dispatched_at).getTime() > 2 * s.timeout_minutes * 60_000) {
          console.log(`::warning::[${s.id}] still queued after ${2 * s.timeout_minutes} min; giving up`);
          await ext.post(`/repos/${s.repository}/actions/runs/${s.remote_run_id}/cancel`).catch(() => {});
          s.remote_conclusion = 'queue_timeout';
          s.error = 'remote run never started (no runner available?)';
        }
      } catch (e) {
        // A single bad poll must not take the whole fan-out down; give up on this repo only
        // when the credential is gone or the run vanished.
        console.log(`::warning::[${s.id}] poll: ${e.message}`);
        if (e instanceof ApiError && (e.status === 401 || e.status === 404)) {
          s.remote_conclusion = 'error'; s.error = `poll failed: ${e.message}`;
        }
      }
    }
    saveState(state);
  }
}

// ---------------------------------------------------------------- report
async function failureDetails(s) {
  if (!s.remote_run_id) return '';
  try {
    const jobs = await ext.get(`/repos/${s.repository}/actions/runs/${s.remote_run_id}/jobs?per_page=100`);
    const bad = jobs.jobs.filter((j) => j.conclusion && j.conclusion !== 'success' && j.conclusion !== 'skipped');
    return bad.map((j) => {
      const steps = (j.steps || []).filter((st) => st.conclusion && st.conclusion !== 'success' && st.conclusion !== 'skipped')
        .map((st) => `    - step ${code(st.name)}: ${st.conclusion}`).join('\n');
      return `- [${code(j.name)}](${j.html_url}): **${j.conclusion}**${steps ? '\n' + steps : ''}`;
    }).join('\n');
  } catch (e) { return `_(job details unavailable: ${code(e.message)})_`; }
}

function checkConclusion(s) {
  if (s.remote_conclusion === 'success') return 'success';
  if (s.failure_policy === 'notify') return 'neutral';
  if (s.remote_conclusion === 'cancelled') return 'cancelled';
  if (s.remote_conclusion === 'timed_out' || s.remote_conclusion === 'queue_timeout') return 'timed_out';
  return 'failure';
}

async function finalizeOne(target, s) {
  s.conclusion = checkConclusion(s);
  const details = s.remote_conclusion === 'success' ? '' : await failureDetails(s);
  const header = `${code(s.repository)} @ ${code(s.ext_ref || '-')} with F´ ${code(`${target.headRepo}@${short(target.headSha)}`)}: **${s.remote_conclusion}**`;
  const lines = [header, ''];
  if (s.remote_run_url) lines.push(`Remote run: ${s.remote_run_url}`);
  if (s.error) lines.push(`Error: ${code(s.error)}`);
  if (s.failure_policy === 'notify' && s.remote_conclusion !== 'success') {
    lines.push('', `This repo is ${code('failure_policy: notify')}: the failure does not block the PR; owners (${s.owners.map((o) => '@' + o).join(' ')}) have been notified.`);
  }
  lines.push('', `Orchestrator run (use "Re-run failed jobs" there to re-dispatch only the failed repos): ${orchestratorUrl}`);
  if (s.check_run_id) {
    await own.patch(`/repos/${repo}/check-runs/${s.check_run_id}`, {
      status: 'completed', conclusion: s.conclusion, details_url: s.remote_run_url || orchestratorUrl,
      output: { title: `${s.remote_conclusion} (${s.repository} @ ${s.ext_ref || '-'})`, summary: lines.join('\n'), text: details || undefined },
    });
  }
  if (s.failure_policy === 'notify') await notifyOwners(target, s, details).catch((e) => console.log(`::warning::[${s.id}] notify: ${e.message}`));
}

// notify policy: one PR comment per ext repo (updated in place), or one tracking issue when
// there is no PR (closed again once the repo passes). Only comments/issues authored by the
// Actions bot are considered ours, so nobody can plant a marker to suppress notifications.
async function ensureLabel() {
  if (await own.exists(`/repos/${repo}/labels/${NOTIFY_LABEL}`)) return;
  await own.post(`/repos/${repo}/labels`, { name: NOTIFY_LABEL, color: 'ededed', description: 'Opened by the external-CI orchestrator' }).catch(() => {});
}

async function notifyOwners(target, s, details) {
  const marker = `<!-- ext-ci-notify:${s.id} -->`;
  const mentions = s.owners.map((o) => '@' + o).join(' ');
  const failed = s.remote_conclusion !== 'success';
  const ours = (x) => x.user && x.user.login === BOT_LOGIN && x.body && x.body.includes(marker);
  if (target.pr) {
    let existing = null;
    for (let page = 1; page <= 10 && !existing; page++) {
      const comments = await own.get(`/repos/${repo}/issues/${target.pr}/comments?per_page=100&page=${page}`);
      existing = comments.find(ours) || null;
      if (comments.length < 100) break;
    }
    if (!failed && !existing) return;
    const body = failed
      ? `${marker}\n### External CI: ${code(s.id)} failed (non-blocking)\n\n${code(s.repository)} @ ${code(s.ext_ref)} does not pass against ${short(target.headSha)}: ${s.remote_run_url || code(s.error)}\n\n${details}\n\n${mentions} — this repo is ${code('failure_policy: notify')}, so the PR is not blocked, but it needs a look.`
      : `${marker}\n### External CI: ${code(s.id)} passing again\n\n${code(s.repository)} @ ${code(s.ext_ref)} passes against ${short(target.headSha)}: ${s.remote_run_url}`;
    if (existing) await own.patch(`/repos/${repo}/issues/comments/${existing.id}`, { body });
    else await own.post(`/repos/${repo}/issues/${target.pr}/comments`, { body });
    return;
  }
  const title = `[ext-ci] ${s.id} failing on ${target.headBranch}`;
  await ensureLabel();
  const issues = await own.get(`/repos/${repo}/issues?state=open&labels=${NOTIFY_LABEL}&creator=${encodeURIComponent(BOT_LOGIN)}&per_page=100`);
  const existing = issues.find((i) => !i.pull_request && ours(i) && i.title === title);
  if (failed) {
    const body = `${marker}\n${code(s.repository)} @ ${code(s.ext_ref)} fails against ${code(target.headBranch)} (${short(target.headSha)}): ${s.remote_run_url || code(s.error)}\n\n${details}\n\n${mentions}\n\nThis issue is closed automatically once the repo passes again.`;
    if (existing) await own.post(`/repos/${repo}/issues/${existing.number}/comments`, { body: `Still failing at ${short(target.headSha)}: ${s.remote_run_url || code(s.error)}` });
    else await own.post(`/repos/${repo}/issues`, { title, body, labels: [NOTIFY_LABEL] });
  } else if (existing) {
    await own.post(`/repos/${repo}/issues/${existing.number}/comments`, { body: `Passing again at ${short(target.headSha)}: ${s.remote_run_url}` });
    await own.patch(`/repos/${repo}/issues/${existing.number}`, { state: 'closed', state_reason: 'completed' });
  }
}

async function carryForward(manifest, target, { entry, prior }, state) {
  state.repos[entry.id] = {
    id: entry.id, repository: entry.repository, failure_policy: entry.failure_policy || 'block', carried_forward: true,
    check_run_id: prior.id, remote_run_url: prior.details_url, remote_conclusion: 'success', conclusion: 'success',
  };
  await own.patch(`/repos/${repo}/check-runs/${prior.id}`, {
    output: {
      title: prior.output.title,
      summary: `${prior.output.summary}\n\n_Not re-run in attempt ${attempt} of ${orchestratorUrl}: previous success carried forward._`,
    },
  });
  console.log(`[${entry.id}] carried forward success from ${prior.details_url}`);
}

async function skipAll(manifest, target, repos, state) {
  for (const entry of repos) {
    const check = await own.post(`/repos/${repo}/check-runs`, {
      name: checkName(manifest, entry.id), head_sha: target.headSha, status: 'completed', conclusion: 'skipped',
      external_id: ourExternalId(entry.id), details_url: orchestratorUrl,
      output: { title: 'Not dispatched: no relevant files changed', summary: `Only paths matching ${code((manifest.paths_ignore || []).join(', '))} changed.\n\nOrchestrator: ${orchestratorUrl}` },
    });
    state.repos[entry.id] = { id: entry.id, repository: entry.repository, failure_policy: entry.failure_policy || 'block', skipped: true, check_run_id: check.id, remote_conclusion: 'skipped', conclusion: 'skipped' };
  }
}

function writeSummary(target, state) {
  const rows = Object.values(state.repos).map((s) =>
    `| ${code(s.id)} | ${s.repository} | ${code(s.ext_ref || '-')} | ${s.failure_policy} | ${s.carried_forward ? 'carried forward' : s.remote_conclusion} | ${s.conclusion} | ${s.remote_run_url ? `[run](${s.remote_run_url})` : (s.error ? code(s.error) : '')} |`);
  const md = [
    `### External CI for ${code(`${target.headRepo}@${short(target.headSha)}`)}${target.pr ? ` (PR #${target.pr})` : ` (${target.headBranch})`} — attempt ${attempt}`,
    '', '| id | repository | ext ref | policy | remote | check | link |', '|---|---|---|---|---|---|---|', ...rows, '',
  ].join('\n');
  if (env.GITHUB_STEP_SUMMARY) fs.appendFileSync(env.GITHUB_STEP_SUMMARY, md);
  console.log(md);
  fs.writeFileSync(RESULTS_FILE, JSON.stringify({ target, attempt, orchestrator_url: orchestratorUrl, repos: state.repos }, null, 2));
}

// ---------------------------------------------------------------- modes
async function run() {
  const target = await resolveTarget();
  console.log(`target: ${JSON.stringify(target)}`);
  const manifest = await loadManifest(target.baseRef);
  // Fork preflight: a fork of F´ that copies this workflow must not dispatch into the ext repos.
  if (manifest.upstream_repository && manifest.upstream_repository !== repo && eventName !== 'workflow_dispatch') {
    console.log(`::notice::${repo} is not ${manifest.upstream_repository}; external CI is only dispatched from upstream. Use workflow_dispatch to run it here deliberately.`);
    return;
  }
  const { carried, dispatch } = await selectRepos(manifest, target);
  const state = { target, attempt, repos: {} };
  saveState(state);
  for (const c of carried) await carryForward(manifest, target, c, state);
  if (eventName !== 'workflow_dispatch' && !relevantChange(await changedFiles(target), manifest)) {
    console.log(`no relevant files changed; marking [${dispatch.map((e) => e.id)}] as skipped`);
    await skipAll(manifest, target, dispatch, state);
    writeSummary(target, state);
    return;
  }
  console.log(`dispatch: [${dispatch.map((e) => e.id)}]  carried forward: [${carried.map((c) => c.entry.id)}]`);
  for (const entry of dispatch) await dispatchOne(manifest, entry, target, state);
  await correlate(state);
  await pollAll(state);
  for (const s of Object.values(state.repos)) {
    if (s.carried_forward) continue;
    try { await finalizeOne(target, s); }
    catch (e) { console.log(`::error::[${s.id}] finalize: ${e.message}`); s.conclusion = s.conclusion || 'failure'; }
    saveState(state);
  }
  writeSummary(target, state);
  const blocking = Object.values(state.repos).filter((s) => s.failure_policy !== 'notify' && s.conclusion !== 'success' && s.conclusion !== 'skipped');
  if (blocking.length) {
    console.log(`::error::blocking external repos failed: ${blocking.map((s) => s.id).join(', ')}`);
    process.exit(1);
  }
}

// Runs under `if: always()`: whatever the run step left open (crash, cancellation, timeout) is
// closed here so no Check Run stays in_progress and no remote run keeps burning runners.
async function finalize() {
  const state = loadState();
  if (!state) return console.log('no state file; nothing to finalize');
  const cancelled = env.JOB_STATUS === 'cancelled';
  for (const s of Object.values(state.repos)) {
    if (s.carried_forward || s.skipped || s.conclusion) continue;
    if (s.remote_run_id && !s.remote_conclusion) {
      console.log(`[${s.id}] cancelling ${s.remote_run_url}`);
      await ext.post(`/repos/${s.repository}/actions/runs/${s.remote_run_id}/cancel`).catch((e) => console.log(`  ${e.message}`));
    }
    if (s.check_run_id) {
      const current = await own.get(`/repos/${repo}/check-runs/${s.check_run_id}`).catch(() => null);
      if (current && current.status === 'completed') continue;
      const conclusion = cancelled ? 'cancelled' : (s.failure_policy === 'notify' ? 'neutral' : 'failure');
      const why = cancelled ? 'The orchestrator run was cancelled' : `The orchestrator run ended before this repo was reported (${code(s.error || 'orchestrator error')})`;
      await own.patch(`/repos/${repo}/check-runs/${s.check_run_id}`, {
        status: 'completed', conclusion,
        output: { title: cancelled ? 'Cancelled by orchestrator' : 'Orchestrator error', summary: `${why}; remote run ${s.remote_run_url || '(not started)'} was cancelled.\n\n${orchestratorUrl}` },
      }).catch((e) => console.log(`  ${e.message}`));
    }
  }
}

await (mode === 'finalize' ? finalize() : run());
