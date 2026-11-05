# External CI (`ext-ci`)

F´ does not build the `fprime-community` projects itself any more: `.github/workflows/ext-ci.yml`
dispatches each project's own `ci.yml` with the F´ revision under test and reports the result as a
Check Run named `ext-ci / <id>` on the F´ commit. `ext-ci-trigger.yml` is the trivial `pull_request`
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

`GITHUB_TOKEN` (`checks: write`, this repository only) reports results; the repository secret
`EXT_CI_DISPATCH_TOKEN` (`actions: write` on the external repositories, no permission here) starts the
remote runs and is exposed only to the two orchestrator steps.
