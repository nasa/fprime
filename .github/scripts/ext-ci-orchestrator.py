#!/usr/bin/env python3
"""External-CI orchestrator (standard library only, Python >= 3.10).

  python3 ext-ci-orchestrator.py run       dispatch, poll, report
  python3 ext-ci-orchestrator.py finalize  close anything still open (runs under if: always())

For every repo in .github/ext-ci-manifest.json: create a Check Run on the F´ commit under test,
dispatch the repo's own ci.yml with that commit, poll the remote run, close the Check Run with
its result. A re-run of the orchestrator re-dispatches every repo.

Two credentials, never mixed:
  GITHUB_TOKEN    this repo only (checks: write) - reports results
  DISPATCH_TOKEN  external repos only (actions: write; contents: read too if an external repo
                  is private, for the branch lookup) - starts remote runs
"""
import json
import os
import re
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from datetime import datetime, timedelta, timezone
from fnmatch import fnmatch

env = os.environ
MODE = sys.argv[1] if len(sys.argv) > 1 else "run"
API = env.get("GITHUB_API_URL", "https://api.github.com")
SERVER = env.get("GITHUB_SERVER_URL", "https://github.com")
STATE_FILE = "ext-ci-state.json"
RESULTS_FILE = "ext-ci-results.json"
MANIFEST_PATH = ".github/ext-ci-manifest.json"
POLL_S = int(env.get("POLL_INTERVAL_SECONDS") or 60)
ORCHESTRATOR_FAULTS = ("dispatch_failed", "queue_timeout", "error")  # remote_conclusion values we made up

REPO = env.get("GITHUB_REPOSITORY")
RUN_ID = env.get("GITHUB_RUN_ID")
ATTEMPT = int(env.get("GITHUB_RUN_ATTEMPT") or 1)
EVENT_NAME = env.get("GITHUB_EVENT_NAME")
with open(env["GITHUB_EVENT_PATH"]) as _f:
    EVENT = json.load(_f)
ORCHESTRATOR_URL = f"{SERVER}/{REPO}/actions/runs/{RUN_ID}" + (f"/attempts/{ATTEMPT}" if ATTEMPT > 1 else "")


def log(msg: str) -> None:
    """Print one line to the job log, unbuffered so it interleaves correctly with GitHub's step output."""
    print(msg, flush=True)


def code(text: object) -> str:
    """Render untrusted text (job/step names, branch names, errors) as a markdown code span.

    Backticks and newlines are replaced so remote-controlled text cannot inject markdown or links into
    Check Run output. Returns the quoted string."""
    return "`" + re.sub(r"[`\r\n]", " ", str(text)) + "`"


def q(text: object) -> str:
    """Percent-encode one URL path segment (branch names may contain '/')."""
    return urllib.parse.quote(str(text), safe="")


def now() -> datetime:
    """Current time as a timezone-aware UTC datetime."""
    return datetime.now(timezone.utc)


def parse_ts(s: str) -> datetime:
    """Parse a GitHub API timestamp such as '2026-01-01T12:00:00Z' into an aware UTC datetime."""
    return datetime.strptime(s, "%Y-%m-%dT%H:%M:%SZ").replace(tzinfo=timezone.utc)


def fprime_rev(target: dict) -> str:
    """Short 'owner/repo@sha7' label of the F´ revision under test, for messages."""
    return f"{target['head_repo']}@{target['head_sha'][:7]}"


# ---------------------------------------------------------------- GitHub REST client
class ApiError(Exception):
    """A GitHub API call failed. `status` is the HTTP status, or 0 for a network error / missing token."""

    def __init__(self, msg: str, status: int = 0) -> None:
        super().__init__(msg)
        self.status = status


class Client:
    """Minimal GitHub REST client bound to one credential; `label` names it in logs and errors."""

    def __init__(self, token: str | None, label: str) -> None:
        self.token, self.label = token, label

    def request(self, method: str, url: str, body=None, idempotent: bool = True) -> tuple:
        """Perform one API call and return (HTTP status, parsed body).

        Never raises on a 4xx/5xx status: callers decide what a status means (see `api`, `exists`).
        Retries rate limits always; network errors and 5xx only when the call is idempotent (a
        workflow dispatch is not: the run may have started). Raises ApiError when the token is
        missing, when a non-idempotent call hits a network error, or after five failed attempts."""
        if not self.token:
            raise ApiError(f"{self.label} token missing (secret not configured?)")
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
        for i in range(5):
            req = urllib.request.Request(API + url, data=data, method=method, headers=headers)
            try:
                with urllib.request.urlopen(req, timeout=60) as res:
                    status, hdrs, text = res.status, res.headers, res.read().decode()
            except urllib.error.HTTPError as e:
                status, hdrs, text = e.code, e.headers, e.read().decode(errors="replace")
            except (urllib.error.URLError, OSError) as e:
                if not idempotent or i == 4:
                    raise ApiError(f"{self.label}: {method} {url} -> network error {e}")
                time.sleep(2 * 2**i)
                continue
            limited = status == 429 or (
                status == 403 and (hdrs.get("x-ratelimit-remaining") == "0" or re.search("rate limit", text, re.I))
            )
            if (limited or (status >= 500 and idempotent)) and i < 4:
                wait = 2 * 2**i
                if hdrs.get("retry-after"):
                    wait = int(hdrs["retry-after"])
                elif limited and hdrs.get("x-ratelimit-reset"):
                    wait = max(wait, int(hdrs["x-ratelimit-reset"]) - int(time.time()) + 1)
                log(f"::warning::{self.label}: {method} {url} -> {status}; retrying in {wait} s")
                time.sleep(min(wait, 15 * 60))
                continue
            try:
                return status, json.loads(text) if text else None
            except ValueError:
                return status, text
        raise AssertionError("unreachable")

    def api(self, method: str, url: str, body=None, idempotent: bool = True):
        """`request` that raises ApiError on any non-2xx status; returns the parsed body."""
        status, data = self.request(method, url, body, idempotent)
        if not 200 <= status < 300:
            raise ApiError(f"{self.label}: {method} {url} -> {status} {json.dumps(data)[:300]}", status)
        return data

    def get(self, url: str):
        """GET; returns the parsed body or raises ApiError."""
        return self.api("GET", url)

    def post(self, url: str, body=None, idempotent: bool = True):
        """POST a JSON body; returns the parsed response or raises ApiError."""
        return self.api("POST", url, body, idempotent)

    def patch(self, url: str, body):
        """PATCH a JSON body; returns the parsed response or raises ApiError."""
        return self.api("PATCH", url, body)

    def exists(self, url: str) -> bool:
        """True on 200, False on 404; any other status raises ApiError, never silently "absent"."""
        status, _ = self.request("GET", url)
        if status == 200:
            return True
        if status == 404:
            return False
        raise ApiError(f"{self.label}: GET {url} -> {status}", status)


own = Client(env.get("GITHUB_TOKEN"), "GITHUB_TOKEN")
ext = Client(env.get("DISPATCH_TOKEN"), "DISPATCH_TOKEN")


# ---------------------------------------------------------------- what to test
def resolve_target() -> dict:
    """Decide which F´ revision is under test from the triggering event.

    Returns a dict with: `pr` (number or None), `head_repo` / `head_sha` / `head_branch` (the revision
    to test; PR-author-controlled: forwarded verbatim as the dispatch inputs the external `ci.yml`
    checks out, and otherwise used only to find the PR and as display text),
    `base_ref` (trusted branch the run was started from; drives the external branch lookup) and
    Raises RuntimeError when a PR run no longer matches an open PR or `pr_number` is not numeric,
    ApiError when a manual PR/ref does not exist."""
    t = {"pr": None, "head_repo": REPO, "head_sha": None, "head_branch": None, "base_ref": None}
    if EVENT_NAME == "workflow_run":
        wr = EVENT["workflow_run"]
        # Trust only the event payload: nothing produced by the (untrusted) trigger run is read.
        t["head_sha"], t["head_branch"] = wr["head_sha"], wr["head_branch"]
        t["head_repo"] = wr["head_repository"]["full_name"]
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
            # base_ref (ext branch lookup) is the branch this run was started from, not the ref under test.
            t.update(head_sha=commit["sha"], head_branch=ref, base_ref=env["GITHUB_REF_NAME"])
    else:
        raise RuntimeError(f"unsupported event: {EVENT_NAME}")
    return t


def changed_files(target: dict) -> list[str] | None:
    """List the files touched by the PR; None when there is no PR (manual ref runs), which callers
    treat as "every repo is relevant"."""
    if not target["pr"]:
        return None
    files = []
    for page in range(1, 31):  # GitHub lists at most 3000 files per PR
        batch = own.get(f"/repos/{REPO}/pulls/{target['pr']}/files?per_page=100&page={page}")
        files += [f["filename"] for f in batch]
        if len(batch) < 100:
            break
    return files


def relevant_change(files: list[str] | None, manifest: dict) -> bool:
    """False only when every changed file matches a `paths_ignore` glob; an unknown list (None) counts
    as relevant.

    paths_ignore lives here, not on the trigger workflow, so that filtered PRs still get a (skipped)
    Check Run per repo and required checks can be satisfied."""
    ignore = manifest.get("paths_ignore", [])
    if files is None or not ignore:
        return True
    # fnmatch's '*' also matches '/', so 'docs/**' behaves like 'docs/*' (good enough here).
    ignored = lambda f: any(fnmatch(f, g) for g in ignore)  # noqa: E731
    return not all(ignored(f) for f in files)


def select_repos(manifest: dict, target: dict) -> list[dict]:
    """Return the manifest entries to run: all of them, narrowed to the `repos` input ids on a manual run
    (unknown id raises RuntimeError) and to entries with `on_pr` true (the default) when a PR is tested,
    manual runs included."""
    repos = manifest["repos"]
    wanted = (env.get("INPUT_REPOS") or "all").strip()
    if EVENT_NAME == "workflow_dispatch" and wanted and wanted != "all":
        ids = [s.strip() for s in wanted.split(",") if s.strip()]
        unknown = [i for i in ids if not any(entry["id"] == i for entry in repos)]
        if unknown:
            raise RuntimeError("unknown manifest ids: " + ", ".join(unknown))
        repos = [entry for entry in repos if entry["id"] in ids]
    if target["pr"]:
        repos = [entry for entry in repos if entry.get("on_pr", True)]
    return repos


def resolve_ext_ref(entry: dict, target: dict) -> str:
    """Return the external repo branch to dispatch: its `pr-<N>` branch if it exists, else a branch
    named like the F´ base branch (`devel`, `release/x`), else the entry's `default_ref` (`main`)."""
    candidates = [target["base_ref"]]
    if target["pr"]:
        candidates.insert(0, f"pr-{target['pr']}")
    for candidate in candidates:
        if candidate and ext.exists(f"/repos/{entry['repository']}/branches/{q(candidate)}"):
            return candidate
    return entry.get("default_ref", "main")


def check_name(manifest: dict, repo_id: str) -> str:
    """Name of the Check Run for one manifest entry: '<check_prefix> / <id>'."""
    return f"{manifest.get('check_prefix', 'ext-ci')} / {repo_id}"


def check_succeeded(manifest: dict, entry: dict, target: dict) -> bool:
    """True when this entry's latest Check Run on the F´ commit (from an earlier attempt) succeeded."""
    runs = own.get(f"/repos/{REPO}/commits/{target['head_sha']}/check-runs"
                   f"?check_name={q(check_name(manifest, entry['id']))}&filter=latest")["check_runs"]
    return any(r["conclusion"] == "success" for r in runs)


def save_state(state: dict) -> None:
    """Write the run state to STATE_FILE; called after every step so `finalize` can recover it."""
    with open(STATE_FILE, "w") as f:
        json.dump(state, f, indent=2)


# ---------------------------------------------------------------- dispatch and poll
def dispatch_one(manifest: dict, entry: dict, target: dict, state: dict) -> None:
    """Start one external repo: create its Check Run (queued), dispatch its workflow with the F´
    revision, mark the check in_progress with the remote run link.

    Records progress in state["repos"][id] (keys: check_run_id, ext_ref, remote_run_id/_url,
    remote_started_at, remote_conclusion, conclusion, error, ...). Never raises: any failure ends
    this repo as `remote_conclusion: dispatch_failed` so the other repos still run."""
    s = state["repos"][entry["id"]] = {
        "id": entry["id"], "repository": entry["repository"], "workflow": entry["workflow"], "ext_ref": None,
        "failure_policy": entry.get("failure_policy", "block"), "timeout_minutes": entry.get("timeout_minutes", 60),
        "dispatched_at": now().strftime("%Y-%m-%dT%H:%M:%SZ"), "check_run_id": None,
        "remote_run_id": None, "remote_run_url": None, "remote_started_at": None,
        "remote_conclusion": None, "conclusion": None, "error": None,
    }
    try:
        # Check Run first, so a credential or API problem on the ext side still ends in a visible
        # failed check rather than a required check that never appears.
        check = own.post(f"/repos/{REPO}/check-runs", {
            "name": check_name(manifest, entry["id"]), "head_sha": target["head_sha"], "status": "queued",
            "details_url": ORCHESTRATOR_URL,
            "output": {"title": f"Dispatching {entry['repository']}",
                       "summary": f"Dispatching {code(entry['workflow'])} on {code(entry['repository'])} with F´ "
                                  f"{code(fprime_rev(target))}.\n\nOrchestrator: {ORCHESTRATOR_URL}"},
        })
        s["check_run_id"] = check["id"]
        save_state(state)
        s["ext_ref"] = resolve_ext_ref(entry, target)
        r = ext.post(f"/repos/{entry['repository']}/actions/workflows/{entry['workflow']}/dispatches", {
            "ref": s["ext_ref"],
            "inputs": {"fprime_repository": target["head_repo"], "fprime_ref": target["head_sha"],
                       "fprime_pr_number": str(target["pr"] or ""), "dispatch_id": f"{RUN_ID}-{ATTEMPT}-{entry['id']}"},
            "return_run_details": True,
        }, idempotent=False)
        if not r or not r.get("workflow_run_id"):
            raise RuntimeError("dispatch accepted but no run id returned")
        s["remote_run_id"], s["remote_run_url"] = r["workflow_run_id"], r.get("html_url")
        log(f"[{entry['id']}] dispatched {entry['repository']}/{entry['workflow']} @ {s['ext_ref']} -> {s['remote_run_url']}")
        try:
            own.patch(f"/repos/{REPO}/check-runs/{s['check_run_id']}", {
                "status": "in_progress", "details_url": s["remote_run_url"],
                "output": {"title": f"Running on {s['repository']} @ {s['ext_ref']}",
                           "summary": f"Remote run: {s['remote_run_url']}\n\nOrchestrator: {ORCHESTRATOR_URL}"},
            })
        except ApiError as e:  # cosmetic: the run exists and is polled regardless
            log(f"::warning::[{entry['id']}] check update: {e}")
    except Exception as e:  # noqa: BLE001 - one repo's failure must not stop the fan-out
        s["error"], s["remote_conclusion"] = f"dispatch failed: {e}", "dispatch_failed"
        log(f"::error::[{entry['id']}] {s['error']}")
    save_state(state)


def cancel_run(s: dict) -> None:
    """Cancel the remote run of one repo; API errors are logged, not raised."""
    try:
        ext.post(f"/repos/{s['repository']}/actions/runs/{s['remote_run_id']}/cancel")
    except ApiError as e:
        log(f"::warning::[{s['id']}] cancel: {e}")


def poll_all(state: dict) -> None:
    """Poll every dispatched run every POLL_S seconds until each has a `remote_conclusion`.

    Sets it to the run's conclusion, `timed_out` (running longer than timeout_minutes, remote run
    cancelled), `queue_timeout` (not started after 2x timeout) or `error` (401/404 while polling).
    Other poll errors are retried on the next tick."""

    def active() -> list[dict]:
        return [s for s in state["repos"].values() if s["remote_run_id"] and not s["remote_conclusion"]]

    while active():
        time.sleep(POLL_S)
        for s in active():
            try:
                run = ext.get(f"/repos/{s['repository']}/actions/runs/{s['remote_run_id']}")
                if run.get("run_started_at") and not s["remote_started_at"]:
                    s["remote_started_at"] = run["run_started_at"]
                timeout = timedelta(minutes=s["timeout_minutes"])
                if run["status"] == "completed":
                    s["remote_conclusion"] = run["conclusion"]
                    log(f"[{s['id']}] completed: {run['conclusion']}")
                elif s["remote_started_at"] and now() - parse_ts(s["remote_started_at"]) > timeout:
                    # Counted from when the remote run started: queue time is not the ext repo's fault.
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
def failure_details(s: dict) -> str:
    """Markdown list of the remote run's failed jobs and steps (empty when there is no remote run)."""
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


def check_conclusion(s: dict) -> str:
    """Map the remote result to the Check Run conclusion: success, neutral (any failure on a
    `failure_policy: notify` repo), cancelled, timed_out or failure."""
    rc = s["remote_conclusion"]
    if rc == "success":
        return "success"
    if s["failure_policy"] == "notify":
        return "neutral"  # visible, but does not block the PR
    if rc == "cancelled":
        return "cancelled"
    if rc in ("timed_out", "queue_timeout"):
        return "timed_out"
    return "failure"


def finalize_one(target: dict, s: dict) -> None:
    """Close one repo's Check Run with its conclusion, summary and failed-job details.

    Sets s["conclusion"] only after the update succeeded, so `finalize` retries it otherwise."""
    conclusion = check_conclusion(s)
    details = "" if s["remote_conclusion"] == "success" else failure_details(s)
    lines = [f"{code(s['repository'])} @ {code(s['ext_ref'] or '-')} with F´ {code(fprime_rev(target))}: "
             f"**{s['remote_conclusion']}**", ""]
    if s["remote_run_url"]:
        lines.append(f"Remote run: {s['remote_run_url']}")
    if s["error"]:
        lines.append(f"Error: {code(s['error'])}")
    if s["failure_policy"] == "notify" and s["remote_conclusion"] != "success":
        lines += ["", f"This repo is {code('failure_policy: notify')}: the failure does not block the PR."]
    lines += ["", f"Orchestrator run: {ORCHESTRATOR_URL}"]
    output = {"title": f"{s['remote_conclusion']} ({s['repository']} @ {s['ext_ref'] or '-'})", "summary": "\n".join(lines)}
    if details:
        output["text"] = details
    own.patch(f"/repos/{REPO}/check-runs/{s['check_run_id']}", {
        "status": "completed", "conclusion": conclusion, "details_url": s["remote_run_url"] or ORCHESTRATOR_URL,
        "output": output,
    })
    s["conclusion"] = conclusion


def skip_all(manifest: dict, target: dict, repos: list[dict], state: dict) -> None:
    """Create every repo's Check Run directly as completed/skipped (only `paths_ignore` files changed)."""
    ignored = ", ".join(manifest.get("paths_ignore", []))
    for entry in repos:
        check = own.post(f"/repos/{REPO}/check-runs", {
            "name": check_name(manifest, entry["id"]), "head_sha": target["head_sha"],
            "status": "completed", "conclusion": "skipped", "details_url": ORCHESTRATOR_URL,
            "output": {"title": "Not dispatched: no relevant files changed",
                       "summary": f"Only paths matching {code(ignored)} changed.\n\nOrchestrator: {ORCHESTRATOR_URL}"},
        })
        state["repos"][entry["id"]] = {
            "id": entry["id"], "repository": entry["repository"], "failure_policy": entry.get("failure_policy", "block"),
            "check_run_id": check["id"], "remote_conclusion": "skipped", "conclusion": "skipped",
        }


def write_summary(target: dict, state: dict) -> None:
    """Write the per-repo results table to the job summary and the log, and RESULTS_FILE as JSON."""
    rows = []
    for s in state["repos"].values():
        if s.get("remote_run_url"):
            link = f"[run]({s['remote_run_url']})"
        elif s.get("error"):
            link = code(s["error"])
        else:
            link = ""
        rows.append(f"| {code(s['id'])} | {s['repository']} | {code(s.get('ext_ref') or '-')} | {s['failure_policy']} "
                    f"| {s['remote_conclusion']} | {s['conclusion']} | {link} |")
    where = f" (PR #{target['pr']})" if target["pr"] else f" ({code(target['head_branch'])})"
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
def run() -> None:
    """The `run` mode: resolve target, load manifest, fork preflight, select repos, dispatch, poll,
    close checks, write summary. Exits 1 when a `failure_policy: block` repo did not succeed or the
    orchestrator itself failed on any repo."""
    target = resolve_target()
    log(f"target: {json.dumps(target)}")
    with open(MANIFEST_PATH) as f:
        manifest = json.load(f)
    # Fork preflight: a fork of F´ that copies this workflow must not dispatch into the ext repos.
    upstream = manifest.get("upstream_repository")
    if upstream and upstream != REPO and EVENT_NAME != "workflow_dispatch":
        log(f"::notice::{REPO} is not {upstream}; external CI is only dispatched from upstream. "
            "Use workflow_dispatch to run it here deliberately.")
        return
    repos = select_repos(manifest, target)
    if ATTEMPT > 1:  # re-run: only the repos whose check is not green yet
        repos = [e for e in repos if not check_succeeded(manifest, e, target)]
        log(f"attempt {ATTEMPT}: re-dispatching {[e['id'] for e in repos] or 'nothing'}")
    state = {"target": target, "attempt": ATTEMPT, "repos": {}}
    save_state(state)
    if EVENT_NAME != "workflow_dispatch" and not relevant_change(changed_files(target), manifest):
        log("no relevant files changed; marking all repos as skipped")
        skip_all(manifest, target, repos, state)
        write_summary(target, state)
        return
    for entry in repos:
        dispatch_one(manifest, entry, target, state)
    poll_all(state)
    for s in state["repos"].values():
        try:
            finalize_one(target, s)
        except Exception as e:  # noqa: BLE001 - the check stays open and finalize() closes it
            log(f"::error::[{s['id']}] finalize: {e}")
        save_state(state)
    write_summary(target, state)
    # conclusion is None when finalize_one failed: judge on what it would have been. `notify` only
    # softens a remote build failure; an orchestrator-side fault always fails this job.
    blocking = [s["id"] for s in state["repos"].values()
                if (s["conclusion"] or check_conclusion(s)) != "success"
                and (s["failure_policy"] != "notify" or s["remote_conclusion"] in ORCHESTRATOR_FAULTS)]
    if blocking:
        log("::error::blocking external repos failed: " + ", ".join(blocking))
        sys.exit(1)


def finalize() -> None:
    """The `finalize` mode, run under `if: always()`: whatever the run step left open (crash, cancellation, timeout) is
    closed here so no Check Run stays in_progress and no remote run keeps burning runners."""
    if not os.path.exists(STATE_FILE):
        return log("no state file; nothing to finalize")
    with open(STATE_FILE) as f:
        state = json.load(f)
    cancelled = env.get("JOB_STATUS") == "cancelled"
    for s in state["repos"].values():
        if s.get("conclusion") or not s.get("check_run_id"):
            continue
        remote = s.get("remote_run_url") or "(not started)"
        if s.get("remote_run_id") and not s.get("remote_conclusion"):
            log(f"[{s['id']}] cancelling {remote}")
            cancel_run(s)
            remote += " (cancelled)"
        try:
            if own.get(f"/repos/{REPO}/check-runs/{s['check_run_id']}")["status"] == "completed":
                continue
        except ApiError:
            pass
        if cancelled:
            conclusion, title, why = "cancelled", "Cancelled by orchestrator", "The orchestrator run was cancelled"
        elif s.get("remote_conclusion"):  # reported, but the check update itself failed in run
            conclusion, title = check_conclusion(s), f"{s['remote_conclusion']} ({s['repository']} @ {s.get('ext_ref') or '-'})"
            why = "Closed by the finalize step after the check update failed"
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
            log(f"::warning::[{s['id']}] check update: {e}")


if __name__ == "__main__":
    if MODE == "finalize":
        finalize()
    elif MODE == "run":
        run()
    else:
        sys.exit(f"usage: {sys.argv[0]} run|finalize")
