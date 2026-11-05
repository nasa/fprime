# External CI

F´ does not build the `fprime-community` projects itself any more: `.github/workflows/ext-ci.yml`
dispatches each project's own `ci.yml` with the F´ revision under test and reports the result as a
Check Run named `External CI / <id>` on the F´ commit. `ext-ci-trigger.yml` (`External CI Trigger / Approval Gate`) is the trivial `pull_request`
workflow whose completion (after GitHub's fork-approval gate) starts the orchestrator; the orchestrator
also runs for merge groups and on manual dispatch (`repos`, `pr_number` or `fprime_ref` inputs).

## manifest.json

| key | meaning |
|---|---|
| `upstream_repository` | only this repository dispatches automatically; forks run only on manual dispatch |
| `check_prefix` | Check Run name is `<check_prefix> / <id>` |
| `paths_ignore` | globs; a PR touching only these gets `skipped` checks and no dispatch |
| `repos[].id` | Check Run suffix and value for the manual `repos` input |
| `repos[].repository` | external repository (`owner/name`) |
| `repos[].workflow` | workflow file in that repository; must accept the inputs below |
| `repos[].default_ref` | branch to dispatch when neither `pr-<N>` nor a branch named like the F´ base branch exists there |
| `repos[].timeout_minutes` | remote running time allowed, and separately the time it may wait in queue; the remote run is cancelled on timeout. Keep `2 * timeout_minutes` under the job's `timeout-minutes` in ext-ci.yml |
| `repos[].failure_policy` | `block`: a failure is a red check and fails the orchestrator job; `notify`: neutral (non-blocking) check |
| `repos[].on_pr` | `false`: never dispatched for a PR head (merge queue and manual `fprime_ref` runs only) |

Orchestrator-side faults (dispatch error, run never started, polling lost the run) fail the job
whatever the policy. A re-run of the orchestrator re-dispatches only the repos whose check is not green.

## Contract for an external `ci.yml`

- `on: workflow_dispatch` with inputs `fprime_repository`, `fprime_ref` (exact SHA to use as the F´
  dependency), `fprime_pr_number`, `dispatch_id`.
- `permissions: contents: read` (or `{}`), no `${{ secrets.* }}`, no Actions cache writes on dispatched
  runs: the dispatched run executes untrusted F´ PR code in the external repository's own context.
- Repositories with self-hosted or hardware runners: `on_pr: false` in the manifest, ephemeral runners,
  SHA-pinned actions, and a job-level guard on `inputs.fprime_repository` in the workflow.

## Credentials

One GitHub App ("External CI", registered under fprime-community; permissions Actions read/write,
Checks read/write, Contents read, Pull requests read; no webhook) installed twice: on the external
repositories listed in the manifest and on this repository. The workflow mints two 1-hour installation
tokens from the repository secrets `EXT_CI_APP_ID` and `EXT_CI_APP_PRIVATE_KEY`, exposed only to the two
orchestrator steps:

- `DISPATCH_TOKEN` (fprime-community installation) starts and cancels the remote runs. Adding an
  external repository = adding it to that installation and to the manifest.
- `CHECKS_TOKEN` (this repository's installation) creates the Check Runs, so they appear under their own
  "External CI" group in the PR checks list. If the App is not installed here the orchestrator falls
  back to `GITHUB_TOKEN` (`checks: write`) and GitHub files the checks under another workflow's group.
