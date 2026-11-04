#!/usr/bin/env python3
"""External-CI orchestrator (standard library only, Python >= 3.10).

  python3 ext-ci-orchestrator.py run       dispatch, poll, report
  python3 ext-ci-orchestrator.py finalize  close anything still open (runs under if: always())

Two credentials, never mixed:
  GITHUB_TOKEN    this repo only (checks/pull-requests/issues write) - reports results
  DISPATCH_TOKEN  external repos only (actions: write; contents: read too if an external repo
                  is private, for the branch lookup) - starts remote runs
"""
import base64
import json
import os
import re
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from datetime import datetime, timedelta, timezone

env = os.environ
MODE = sys.argv[1] if len(sys.argv) > 1 else "run"
API = env.get("GITHUB_API_URL", "https://api.github.com")
SERVER = env.get("GITHUB_SERVER_URL", "https://github.com")
STATE_FILE = "ext-ci-state.json"
RESULTS_FILE = "ext-ci-results.json"
MANIFEST_PATH = ".github/ext-ci-manifest.json"
POLL_S = int(env.get("POLL_INTERVAL_SECONDS") or 60)
CORRELATE_TIMEOUT_S = 5 * 60
BOT_LOGIN = "github-actions[bot]"
NOTIFY_LABEL = "ext-ci"

REPO = env.get("GITHUB_REPOSITORY")
RUN_ID = env.get("GITHUB_RUN_ID")
ATTEMPT = int(env.get("GITHUB_RUN_ATTEMPT") or 1)
EVENT_NAME = env.get("GITHUB_EVENT_NAME")
with open(env["GITHUB_EVENT_PATH"]) as _f:
    EVENT = json.load(_f)
ORCHESTRATOR_URL = f"{SERVER}/{REPO}/actions/runs/{RUN_ID}" + (f"/attempts/{ATTEMPT}" if ATTEMPT > 1 else "")


def log(msg):
    print(msg, flush=True)


def short(sha):
    return sha[:7]


def code(s):
    # Remote-controlled text (job/step names, branch names) is only ever rendered inside code spans.
    return "`" + re.sub(r"[`\r\n]", " ", str(s)) + "`"


def q(s):
    return urllib.parse.quote(str(s), safe="")


def now():
    return datetime.now(timezone.utc)


def iso(dt):
    return dt.strftime("%Y-%m-%dT%H:%M:%SZ")


def parse_ts(s):
    return datetime.strptime(s, "%Y-%m-%dT%H:%M:%SZ").replace(tzinfo=timezone.utc)


def fprime_rev(target):
    return f"{target['head_repo']}@{short(target['head_sha'])}"


def mentions(s):
    return " ".join("@" + o for o in s["owners"])


# ---------------------------------------------------------------- REST helpers
class ApiError(Exception):
    def __init__(self, msg, status=0):
        super().__init__(msg)
        self.status = status


class Client:
    def __init__(self, token, label):
        self.token, self.label, self.etags = token, label, {}

    def request(self, method, url, body=None, conditional=False, idempotent=True):
        """Returns (status, parsed body). Retries rate limits always, network errors and 5xx only
        when the call is idempotent (a workflow dispatch is not: the run may have started)."""
        if not self.token:
            raise ApiError(f"{self.label} token missing (secret not configured?)")
        full = url if url.startswith("http") else API + url
        headers = {
            "Authorization": f"Bearer {self.token}",
            "Accept": "application/vnd.github+json",
            "X-GitHub-Api-Version": "2022-11-28",
            "User-Agent": "fprime-ext-ci-orchestrator",
        }
        data = None
        if body is not None:
            data = json.dumps(body).encode()
            headers["Content-Type"] = "application/json"
        if conditional and full in self.etags:
            headers["If-None-Match"] = self.etags[full][0]
        i = 0
        while True:
            req = urllib.request.Request(full, data=data, method=method, headers=headers)
            try:
                with urllib.request.urlopen(req, timeout=60) as res:
                    status, hdrs, text = res.status, res.headers, res.read().decode()
            except urllib.error.HTTPError as e:
                status, hdrs, text = e.code, e.headers, e.read().decode(errors="replace")
            except (urllib.error.URLError, OSError) as e:
                if not idempotent or i >= 4:
                    raise ApiError(f"{self.label}: {method} {url} -> network error {e}")
                time.sleep(2 * 2**i)
                i += 1
                continue
            if status == 304 and conditional:
                return 200, self.etags[full][1]
            try:
                parsed = json.loads(text) if text else None
            except ValueError:
                parsed = text
            limited = status == 429 or (
                status == 403 and (hdrs.get("x-ratelimit-remaining") == "0" or re.search("rate limit", text, re.I))
            )
            if (limited or (status >= 500 and idempotent)) and i < 4:
                wait = 2 * 2**i
                retry_after, reset = hdrs.get("retry-after"), hdrs.get("x-ratelimit-reset")
                if retry_after:
                    wait = int(retry_after)
                elif limited and reset:
                    wait = max(wait, int(reset) - int(time.time()) + 1)
                log(f"::warning::{self.label}: {method} {url} -> {status}; retrying in {wait} s")
                time.sleep(min(wait, 15 * 60))
                i += 1
                continue
            if 200 <= status < 300 and conditional and hdrs.get("etag"):
                self.etags[full] = (hdrs.get("etag"), parsed)
            return status, parsed

    def api(self, method, url, body=None, **kw):
        status, data = self.request(method, url, body, **kw)
        if not 200 <= status < 300:
            raise ApiError(f"{self.label}: {method} {url} -> {status} {json.dumps(data)[:300]}", status)
        return data

    def get(self, url, **kw):
        return self.api("GET", url, **kw)

    def post(self, url, body=None, **kw):
        return self.api("POST", url, body, **kw)

    def patch(self, url, body):
        return self.api("PATCH", url, body)

    def exists(self, url):
        # True/False for 200/404; anything else is an error, never silently "absent".
        status, _ = self.request("GET", url)
        if status == 200:
            return True
        if status == 404:
            return False
        raise ApiError(f"{self.label}: GET {url} -> {status}", status)


own = Client(env.get("GITHUB_TOKEN"), "GITHUB_TOKEN")
ext = Client(env.get("DISPATCH_TOKEN"), "DISPATCH_TOKEN")


# ---------------------------------------------------------------- target resolution
def resolve_target():
    """What F´ revision are we testing, and which PR (if any) gets the Check Runs?"""
    t = {"pr": None, "head_repo": REPO, "head_sha": None, "head_branch": None, "base_ref": None, "before_sha": None}
    if EVENT_NAME == "workflow_run":
        wr = EVENT["workflow_run"]
        # Trust only the event payload: nothing produced by the (untrusted) trigger run is read.
        t["head_sha"] = wr["head_sha"]
        t["head_repo"] = wr["head_repository"]["full_name"]
        t["head_branch"] = wr["head_branch"]
        if wr["event"] == "merge_group":
            # Merge-queue branch gh-readonly-queue/<base>/pr-<N>-<sha>: checks go on the queue SHA, no PR.
            m = re.match(r"^gh-readonly-queue/(.+?)/pr-\d+-", t["head_branch"] or "")
            t["base_ref"] = m.group(1) if m else env.get("GITHUB_REF_NAME")
            return t
        # workflow_run.pull_requests is empty for fork PRs; find the PR by commit, then by head ref.
        prs = own.get(f"/repos/{REPO}/commits/{t['head_sha']}/pulls?per_page=20")
        pr = next((p for p in prs if p["state"] == "open" and p["head"]["sha"] == t["head_sha"]
                   and p["base"]["repo"]["full_name"] == REPO), None)
        if not pr:
            owner = t["head_repo"].split("/")[0]
            prs = own.get(f"/repos/{REPO}/pulls?state=open&head={q(owner + ':' + t['head_branch'])}")
            pr = next((p for p in prs if p["head"]["sha"] == t["head_sha"]), None)
        if not pr:
            raise RuntimeError(f"no open PR found for {t['head_repo']}@{t['head_sha']} ({t['head_branch']}); "
                               "the PR may have been updated or closed")
        t["pr"], t["base_ref"] = pr["number"], pr["base"]["ref"]
    elif EVENT_NAME == "workflow_dispatch":
        pr_number = (env.get("INPUT_PR_NUMBER") or "").strip()
        if pr_number:
            if not pr_number.isdigit():
                raise RuntimeError(f"pr_number must be numeric, got {pr_number!r}")
            pr = own.get(f"/repos/{REPO}/pulls/{pr_number}")
            t.update(pr=pr["number"], head_sha=pr["head"]["sha"], head_repo=pr["head"]["repo"]["full_name"],
                     head_branch=pr["head"]["ref"], base_ref=pr["base"]["ref"])
        else:
            ref = (env.get("INPUT_FPRIME_REF") or "").strip() or env["GITHUB_REF_NAME"]
            commit = own.get(f"/repos/{REPO}/commits/{q(ref)}")
            t.update(head_sha=commit["sha"], head_branch=ref, base_ref=ref)
    else:  # push, schedule
        t.update(head_sha=env["GITHUB_SHA"], head_branch=env["GITHUB_REF_NAME"], base_ref=env["GITHUB_REF_NAME"])
        before = EVENT.get("before")
        if EVENT_NAME == "push" and before and not re.fullmatch("0+", before):
            t["before_sha"] = before
    return t


def load_manifest(ref):
    """Manifest is read from the base branch of the PR (the orchestrator itself always runs from
    the default branch), so release/** PRs use their own branch's manifest. Only a missing file
    falls back to the checked-out copy; any other error is fatal."""
    try:
        file = own.get(f"/repos/{REPO}/contents/{MANIFEST_PATH}?ref={q(ref)}")
        return json.loads(base64.b64decode(file["content"]))
    except ApiError as e:
        if e.status != 404:
            raise
        log(f"::warning::manifest missing at {ref}; using the checked-out copy")
        with open(MANIFEST_PATH) as f:
            return json.load(f)


def check_name(manifest, repo_id):
    return f"{manifest.get('check_prefix', 'ext-ci')} / {repo_id}"


def our_external_id(repo_id):
    return f"{RUN_ID}-{repo_id}"  # same across attempts of one run


# ---------------------------------------------------------------- change filter
# paths-ignore lives here, not on the trigger workflow, so that filtered PRs still get a
# (skipped) Check Run per repo and required checks can be satisfied.
def glob_to_regex(glob):
    out, i = "^", 0
    while i < len(glob):
        c = glob[i]
        if c == "*":
            if glob[i + 1:i + 2] == "*":
                out += ".*"
                i += 1
                if glob[i + 1:i + 2] == "/":
                    i += 1
            else:
                out += "[^/]*"
        elif c == "?":
            out += "[^/]"
        else:
            out += re.escape(c)
        i += 1
    return re.compile(out + "$")


def changed_files(target):
    if target["pr"]:
        url = f"/repos/{REPO}/pulls/{target['pr']}/files?per_page=100"
    elif target["before_sha"]:
        url = f"/repos/{REPO}/compare/{target['before_sha']}...{target['head_sha']}?per_page=100"
    else:
        return None  # unknown -> run everything
    files = []
    for page in range(1, 31):
        r = own.get(f"{url}&page={page}")
        batch = r if target["pr"] else r.get("files", [])
        files += [f["filename"] for f in batch]
        if len(batch) < 100:
            break
    return files


def relevant_change(files, manifest):
    ignore = [glob_to_regex(g) for g in manifest.get("paths_ignore", [])]
    if files is None or not ignore:
        return True
    return any(not any(rx.match(f) for rx in ignore) for f in files)


# ---------------------------------------------------------------- selection
def latest_own_check_run(name, repo_id, sha):
    r = own.get(f"/repos/{REPO}/commits/{sha}/check-runs?check_name={q(name)}&filter=all&per_page=50")
    # Only reuse a check this very orchestrator run created (external_id stamped with our run id).
    ours = [c for c in r["check_runs"]
            if c.get("external_id") == our_external_id(repo_id) and (c.get("app") or {}).get("slug") == "github-actions"]
    return max(ours, key=lambda c: c["id"], default=None)


def select_repos(manifest, target):
    repos = manifest["repos"]
    wanted = (env.get("INPUT_REPOS") or "all").strip()
    if EVENT_NAME == "workflow_dispatch" and wanted and wanted != "all":
        ids = [s.strip() for s in wanted.split(",") if s.strip()]
        unknown = [i for i in ids if not any(r["id"] == i for r in repos)]
        if unknown:
            raise RuntimeError("unknown manifest ids: " + ", ".join(unknown))
        repos = [r for r in repos if r["id"] in ids]
    elif target["pr"]:
        repos = [r for r in repos if r.get("on_pr", True)]
    # Failed-only re-run: on attempt > 1 keep every repo whose Check Run from attempt 1..n-1 of
    # this same run already succeeded, and dispatch only the rest.
    carried, dispatch = [], []
    force_all = (env.get("INPUT_FORCE_ALL") or "") == "true"
    for entry in repos:
        prior = None
        if ATTEMPT > 1 and not force_all:
            prior = latest_own_check_run(check_name(manifest, entry["id"]), entry["id"], target["head_sha"])
        if prior and prior["status"] == "completed" and prior["conclusion"] == "success":
            carried.append((entry, prior))
        else:
            dispatch.append(entry)
    return carried, dispatch


def resolve_ext_ref(entry, target):
    """pr-<N> branch on the ext repo, else the F´ base branch name, else the manifest default."""
    for c in (f"pr-{target['pr']}" if target["pr"] else None, target["base_ref"]):
        if c and ext.exists(f"/repos/{entry['repository']}/branches/{q(c)}"):
            return c
    return entry.get("default_ref", "main")


# ---------------------------------------------------------------- state
def save_state(state):
    with open(STATE_FILE, "w") as f:
        json.dump(state, f, indent=2)


def load_state():
    if not os.path.exists(STATE_FILE):
        return None
    with open(STATE_FILE) as f:
        return json.load(f)


# ---------------------------------------------------------------- dispatch
def dispatch_one(manifest, entry, target, state):
    dispatch_id = f"{RUN_ID}-{ATTEMPT}-{entry['id']}"
    s = state["repos"][entry["id"]] = {
        "id": entry["id"], "repository": entry["repository"], "workflow": entry["workflow"], "ext_ref": None,
        "failure_policy": entry.get("failure_policy", "block"), "owners": entry.get("owners", []),
        "timeout_minutes": entry.get("timeout_minutes", 60), "dispatch_id": dispatch_id, "dispatched_at": iso(now()),
        "check_run_id": None, "remote_run_id": None, "remote_run_url": None, "remote_started_at": None,
        "remote_conclusion": None, "conclusion": None, "error": None,
    }
    try:
        # Check Run first, so a credential or API problem on the ext side still ends in a visible
        # failed check rather than a required check that never appears.
        check = own.post(f"/repos/{REPO}/check-runs", {
            "name": check_name(manifest, entry["id"]), "head_sha": target["head_sha"], "status": "queued",
            "external_id": our_external_id(entry["id"]), "details_url": ORCHESTRATOR_URL,
            "output": {
                "title": f"Dispatching {entry['repository']}",
                "summary": f"Dispatching {code(entry['workflow'])} on {code(entry['repository'])} with F´ "
                           f"{code(fprime_rev(target))}.\n\nOrchestrator: {ORCHESTRATOR_URL}",
            },
        })
        s["check_run_id"] = check["id"]
        save_state(state)
        s["ext_ref"] = resolve_ext_ref(entry, target)
        try:
            r = ext.post(f"/repos/{entry['repository']}/actions/workflows/{entry['workflow']}/dispatches", {
                "ref": s["ext_ref"],
                "inputs": {
                    "fprime_repository": target["head_repo"], "fprime_ref": target["head_sha"],
                    "fprime_pr_number": str(target["pr"] or ""), "dispatch_id": dispatch_id,
                },
                "return_run_details": True,
            }, idempotent=False)
        except ApiError as e:
            if 0 < e.status < 500:
                raise
            # Lost response: the run may exist; correlate() finds it by dispatch_id or times out.
            log(f"::warning::[{entry['id']}] dispatch outcome unknown ({e})")
            r = None
        if r and r.get("workflow_run_id"):
            s["remote_run_id"], s["remote_run_url"] = r["workflow_run_id"], r.get("html_url")
            mark_running(s)
        where = f" -> {s['remote_run_url']}" if s["remote_run_url"] else f" (dispatch_id={dispatch_id})"
        log(f"[{entry['id']}] dispatched {entry['repository']}/{entry['workflow']} @ {s['ext_ref']}{where}")
    except Exception as e:  # noqa: BLE001 - one repo's failure must not stop the fan-out
        s["error"], s["remote_conclusion"] = f"dispatch failed: {e}", "dispatch_failed"
        log(f"::error::[{entry['id']}] {s['error']}")
    save_state(state)


def mark_running(s):
    # A failed status update is not a failed dispatch: the remote run exists and is polled anyway.
    try:
        own.patch(f"/repos/{REPO}/check-runs/{s['check_run_id']}", {
            "status": "in_progress", "details_url": s["remote_run_url"],
            "output": {"title": f"Running on {s['repository']} @ {s['ext_ref']}",
                       "summary": f"Remote run: {s['remote_run_url']}\n\nOrchestrator: {ORCHESTRATOR_URL}"},
        })
    except ApiError as e:
        log(f"::warning::[{s['id']}] check update: {e}")


def cancel_run(s):
    try:
        ext.post(f"/repos/{s['repository']}/actions/runs/{s['remote_run_id']}/cancel")
    except ApiError as e:
        log(f"  {e}")


def correlate(state):
    """For dispatches without a run id (lost response, or an API server that still answers 204):
    find the run whose title carries our dispatch_id (the ext workflow echoes it into run-name)."""
    pending = [s for s in state["repos"].values() if not s["remote_run_id"] and not s["remote_conclusion"]]
    deadline = time.time() + CORRELATE_TIMEOUT_S
    while pending and time.time() < deadline:
        for s in list(pending):
            try:
                since = iso(parse_ts(s["dispatched_at"]) - timedelta(seconds=60))
                runs = ext.get(f"/repos/{s['repository']}/actions/runs?event=workflow_dispatch"
                               f"&created={q('>=' + since)}&per_page=50")
                run = next((r for r in runs["workflow_runs"]
                            if s["dispatch_id"] in (r.get("display_title") or r.get("name") or "").split()), None)
                if not run:
                    continue
                s["remote_run_id"], s["remote_run_url"] = run["id"], run["html_url"]
                pending.remove(s)
                log(f"[{s['id']}] remote run {run['html_url']}")
                mark_running(s)
                save_state(state)
            except ApiError as e:
                log(f"::warning::[{s['id']}] correlate: {e}")
        if pending:
            time.sleep(15)
    for s in pending:
        s["remote_conclusion"] = "not_found"
        s["error"] = (f"no run with dispatch_id {s['dispatch_id']} appeared on {s['repository']} "
                      f"within {CORRELATE_TIMEOUT_S // 60} min")
        log(f"::error::[{s['id']}] {s['error']}")
    save_state(state)


# ---------------------------------------------------------------- poll
def poll_all(state):
    def active():
        return [s for s in state["repos"].values() if s["remote_run_id"] and not s["remote_conclusion"]]

    while active():
        time.sleep(POLL_S)
        for s in active():
            try:
                run = ext.get(f"/repos/{s['repository']}/actions/runs/{s['remote_run_id']}", conditional=True)
                if run.get("run_started_at") and not s["remote_started_at"]:
                    s["remote_started_at"] = run["run_started_at"]
                timeout = timedelta(minutes=s["timeout_minutes"])
                if run["status"] == "completed":
                    s["remote_conclusion"] = run["conclusion"]
                    log(f"[{s['id']}] completed: {run['conclusion']}")
                elif s["remote_started_at"] and now() - parse_ts(s["remote_started_at"]) > timeout:
                    # Timeout counts from when the remote run started, not from dispatch (queue time is not the ext repo's fault).
                    log(f"::warning::[{s['id']}] timed out after {s['timeout_minutes']} min of running; cancelling remote run")
                    cancel_run(s)
                    s["remote_conclusion"] = "timed_out"
                elif not s["remote_started_at"] and now() - parse_ts(s["dispatched_at"]) > 2 * timeout:
                    log(f"::warning::[{s['id']}] still queued after {2 * s['timeout_minutes']} min; giving up")
                    cancel_run(s)
                    s["remote_conclusion"], s["error"] = "queue_timeout", "remote run never started (no runner available?)"
            except ApiError as e:
                # A single bad poll must not take the whole fan-out down; give up on this repo only
                # when the credential is gone or the run vanished.
                log(f"::warning::[{s['id']}] poll: {e}")
                if e.status in (401, 404):
                    s["remote_conclusion"], s["error"] = "error", f"poll failed: {e}"
        save_state(state)


# ---------------------------------------------------------------- report
def failure_details(s):
    if not s["remote_run_id"]:
        return ""
    try:
        jobs = ext.get(f"/repos/{s['repository']}/actions/runs/{s['remote_run_id']}/jobs?per_page=100")["jobs"]
    except ApiError as e:
        return f"_(job details unavailable: {code(e)})_"
    lines = []
    for j in jobs:
        if j.get("conclusion") in (None, "success", "skipped"):
            continue
        lines.append(f"- [{code(j['name'])}]({j['html_url']}): **{j['conclusion']}**")
        lines += [f"    - step {code(st['name'])}: {st['conclusion']}"
                  for st in j.get("steps", []) if st.get("conclusion") not in (None, "success", "skipped")]
    return "\n".join(lines)


def check_conclusion(s):
    rc = s["remote_conclusion"]
    if rc == "success":
        return "success"
    if s["failure_policy"] == "notify":
        return "neutral"
    if rc == "cancelled":
        return "cancelled"
    if rc in ("timed_out", "queue_timeout"):
        return "timed_out"
    return "failure"


def finalize_one(target, s):
    conclusion = check_conclusion(s)
    details = "" if s["remote_conclusion"] == "success" else failure_details(s)
    lines = [f"{code(s['repository'])} @ {code(s['ext_ref'] or '-')} with F´ {code(fprime_rev(target))}: "
             f"**{s['remote_conclusion']}**", ""]
    if s["remote_run_url"]:
        lines.append(f"Remote run: {s['remote_run_url']}")
    if s.get("error"):
        lines.append(f"Error: {code(s['error'])}")
    if s["failure_policy"] == "notify" and s["remote_conclusion"] != "success":
        lines += ["", f"This repo is {code('failure_policy: notify')}: the failure does not block the PR; "
                      f"owners ({mentions(s)}) have been notified."]
    lines += ["", f'Orchestrator run (use "Re-run failed jobs" there to re-dispatch only the failed repos): {ORCHESTRATOR_URL}']
    if s["check_run_id"]:
        output = {"title": f"{s['remote_conclusion']} ({s['repository']} @ {s['ext_ref'] or '-'})", "summary": "\n".join(lines)}
        if details:
            output["text"] = details
        own.patch(f"/repos/{REPO}/check-runs/{s['check_run_id']}", {
            "status": "completed", "conclusion": conclusion, "details_url": s["remote_run_url"] or ORCHESTRATOR_URL,
            "output": output,
        })
    s["conclusion"] = conclusion  # recorded only once the check is closed, so finalize() retries otherwise
    if s["failure_policy"] == "notify":
        try:
            notify_owners(target, s, details)
        except ApiError as e:
            log(f"::warning::[{s['id']}] notify: {e}")


# notify policy: one PR comment per ext repo (updated in place), or one tracking issue when
# there is no PR (closed again once the repo passes). Only comments/issues authored by the
# Actions bot are considered ours, so nobody can plant a marker to suppress notifications.
def ensure_label():
    if own.exists(f"/repos/{REPO}/labels/{NOTIFY_LABEL}"):
        return
    try:
        own.post(f"/repos/{REPO}/labels",
                 {"name": NOTIFY_LABEL, "color": "ededed", "description": "Opened by the external-CI orchestrator"})
    except ApiError:
        pass


def notify_owners(target, s, details):
    marker = f"<!-- ext-ci-notify:{s['id']} -->"
    failed = s["remote_conclusion"] != "success"
    run_or_error = s["remote_run_url"] or code(s.get("error"))

    def ours(x):
        return (x.get("user") or {}).get("login") == BOT_LOGIN and marker in (x.get("body") or "")

    if target["pr"]:
        existing = None
        for page in range(1, 11):
            comments = own.get(f"/repos/{REPO}/issues/{target['pr']}/comments?per_page=100&page={page}")
            existing = next((c for c in comments if ours(c)), None)
            if existing or len(comments) < 100:
                break
        if not failed and not existing:
            return
        if failed:
            body = (f"{marker}\n### External CI: {code(s['id'])} failed (non-blocking)\n\n"
                    f"{code(s['repository'])} @ {code(s['ext_ref'])} does not pass against {short(target['head_sha'])}: "
                    f"{run_or_error}\n\n{details}\n\n{mentions(s)} — this repo is {code('failure_policy: notify')}, "
                    "so the PR is not blocked, but it needs a look.")
        else:
            body = (f"{marker}\n### External CI: {code(s['id'])} passing again\n\n"
                    f"{code(s['repository'])} @ {code(s['ext_ref'])} passes against {short(target['head_sha'])}: "
                    f"{s['remote_run_url']}")
        if existing:
            own.patch(f"/repos/{REPO}/issues/comments/{existing['id']}", {"body": body})
        else:
            own.post(f"/repos/{REPO}/issues/{target['pr']}/comments", {"body": body})
        return
    title = f"[ext-ci] {s['id']} failing on {target['head_branch']}"
    ensure_label()
    issues = own.get(f"/repos/{REPO}/issues?state=open&labels={NOTIFY_LABEL}&creator={q(BOT_LOGIN)}&per_page=100")
    existing = next((i for i in issues if not i.get("pull_request") and ours(i) and i["title"] == title), None)
    if failed:
        if existing:
            own.post(f"/repos/{REPO}/issues/{existing['number']}/comments",
                     {"body": f"Still failing at {short(target['head_sha'])}: {run_or_error}"})
        else:
            body = (f"{marker}\n{code(s['repository'])} @ {code(s['ext_ref'])} fails against {code(target['head_branch'])} "
                    f"({short(target['head_sha'])}): {run_or_error}\n\n{details}\n\n{mentions(s)}\n\n"
                    "This issue is closed automatically once the repo passes again.")
            own.post(f"/repos/{REPO}/issues", {"title": title, "body": body, "labels": [NOTIFY_LABEL]})
    elif existing:
        own.post(f"/repos/{REPO}/issues/{existing['number']}/comments",
                 {"body": f"Passing again at {short(target['head_sha'])}: {s['remote_run_url']}"})
        own.patch(f"/repos/{REPO}/issues/{existing['number']}", {"state": "closed", "state_reason": "completed"})


def carry_forward(entry, prior, state):
    state["repos"][entry["id"]] = {
        "id": entry["id"], "repository": entry["repository"], "failure_policy": entry.get("failure_policy", "block"),
        "carried_forward": True, "check_run_id": prior["id"], "remote_run_url": prior["details_url"],
        "remote_conclusion": "success", "conclusion": "success",
    }
    own.patch(f"/repos/{REPO}/check-runs/{prior['id']}", {"output": {
        "title": prior["output"]["title"],
        "summary": f"{prior['output']['summary']}\n\n_Not re-run in attempt {ATTEMPT} of {ORCHESTRATOR_URL}: "
                   "previous success carried forward._",
    }})
    log(f"[{entry['id']}] carried forward success from {prior['details_url']}")


def skip_all(manifest, target, repos, state):
    ignored = ", ".join(manifest.get("paths_ignore", []))
    for entry in repos:
        check = own.post(f"/repos/{REPO}/check-runs", {
            "name": check_name(manifest, entry["id"]), "head_sha": target["head_sha"],
            "status": "completed", "conclusion": "skipped",
            "external_id": our_external_id(entry["id"]), "details_url": ORCHESTRATOR_URL,
            "output": {"title": "Not dispatched: no relevant files changed",
                       "summary": f"Only paths matching {code(ignored)} changed.\n\nOrchestrator: {ORCHESTRATOR_URL}"},
        })
        state["repos"][entry["id"]] = {
            "id": entry["id"], "repository": entry["repository"], "failure_policy": entry.get("failure_policy", "block"),
            "skipped": True, "check_run_id": check["id"], "remote_conclusion": "skipped", "conclusion": "skipped",
        }


def write_summary(target, state):
    rows = []
    for s in state["repos"].values():
        remote = "carried forward" if s.get("carried_forward") else s.get("remote_conclusion")
        link = f"[run]({s['remote_run_url']})" if s.get("remote_run_url") else (code(s["error"]) if s.get("error") else "")
        rows.append(f"| {code(s['id'])} | {s['repository']} | {code(s.get('ext_ref') or '-')} | {s['failure_policy']} "
                    f"| {remote} | {s.get('conclusion')} | {link} |")
    where = f" (PR #{target['pr']})" if target["pr"] else f" ({target['head_branch']})"
    md = "\n".join([f"### External CI for {code(fprime_rev(target))}{where} — attempt {ATTEMPT}", "",
                    "| id | repository | ext ref | policy | remote | check | link |",
                    "|---|---|---|---|---|---|---|", *rows, ""])
    if env.get("GITHUB_STEP_SUMMARY"):
        with open(env["GITHUB_STEP_SUMMARY"], "a") as f:
            f.write(md)
    log(md)
    with open(RESULTS_FILE, "w") as f:
        json.dump({"target": target, "attempt": ATTEMPT, "orchestrator_url": ORCHESTRATOR_URL, "repos": state["repos"]},
                  f, indent=2)


# ---------------------------------------------------------------- modes
def run():
    target = resolve_target()
    log(f"target: {json.dumps(target)}")
    manifest = load_manifest(target["base_ref"])
    # Fork preflight: a fork of F´ that copies this workflow must not dispatch into the ext repos.
    upstream = manifest.get("upstream_repository")
    if upstream and upstream != REPO and EVENT_NAME != "workflow_dispatch":
        log(f"::notice::{REPO} is not {upstream}; external CI is only dispatched from upstream. "
            "Use workflow_dispatch to run it here deliberately.")
        return
    carried, dispatch = select_repos(manifest, target)
    state = {"target": target, "attempt": ATTEMPT, "repos": {}}
    save_state(state)
    for entry, prior in carried:
        carry_forward(entry, prior, state)
    ids = [e["id"] for e in dispatch]
    if EVENT_NAME != "workflow_dispatch" and not relevant_change(changed_files(target), manifest):
        log(f"no relevant files changed; marking {ids} as skipped")
        skip_all(manifest, target, dispatch, state)
        write_summary(target, state)
        return
    log(f"dispatch: {ids}  carried forward: {[e['id'] for e, _ in carried]}")
    for entry in dispatch:
        dispatch_one(manifest, entry, target, state)
    correlate(state)
    poll_all(state)
    for s in state["repos"].values():
        if s.get("carried_forward"):
            continue
        try:
            finalize_one(target, s)
        except Exception as e:  # noqa: BLE001 - the check stays open and finalize() closes it
            log(f"::error::[{s['id']}] finalize: {e}")
        save_state(state)
    write_summary(target, state)
    blocking = [s["id"] for s in state["repos"].values()
                if s["failure_policy"] != "notify" and (s.get("conclusion") or check_conclusion(s)) not in ("success", "skipped")]
    if blocking:
        log("::error::blocking external repos failed: " + ", ".join(blocking))
        sys.exit(1)


def finalize():
    """Runs under `if: always()`: whatever the run step left open (crash, cancellation, timeout) is
    closed here so no Check Run stays in_progress and no remote run keeps burning runners."""
    state = load_state()
    if not state:
        log("no state file; nothing to finalize")
        return
    cancelled = env.get("JOB_STATUS") == "cancelled"
    for s in state["repos"].values():
        if s.get("carried_forward") or s.get("skipped") or s.get("conclusion"):
            continue
        remote = s.get("remote_run_url") or "(not started)"
        if s.get("remote_run_id") and not s.get("remote_conclusion"):
            log(f"[{s['id']}] cancelling {remote}")
            cancel_run(s)
            remote += " (cancelled)"
        if not s.get("check_run_id"):
            continue
        try:
            current = own.get(f"/repos/{REPO}/check-runs/{s['check_run_id']}")
        except ApiError:
            current = None
        if current and current["status"] == "completed":
            continue
        if s.get("remote_conclusion") and not cancelled:  # reported, but the check update itself failed in run
            conclusion, title = check_conclusion(s), f"{s['remote_conclusion']} ({s['repository']} @ {s.get('ext_ref') or '-'})"
            why = "Closed by the finalize step after the check update failed"
        elif cancelled:
            conclusion, title, why = "cancelled", "Cancelled by orchestrator", "The orchestrator run was cancelled"
        else:
            conclusion = "neutral" if s["failure_policy"] == "notify" else "failure"
            title = "Orchestrator error"
            why = f"The orchestrator run ended before this repo was reported ({code(s.get('error') or 'orchestrator error')})"
        try:
            own.patch(f"/repos/{REPO}/check-runs/{s['check_run_id']}", {
                "status": "completed", "conclusion": conclusion,
                "output": {"title": title, "summary": f"{why}; remote run {remote}.\n\n{ORCHESTRATOR_URL}"},
            })
        except ApiError as e:
            log(f"  {e}")


if __name__ == "__main__":
    finalize() if MODE == "finalize" else run()
