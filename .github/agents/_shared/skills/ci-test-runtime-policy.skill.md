---
name: ci-test-runtime-policy
description: Use when scanning a PR diff for test-runtime policy violations (network egress, subprocess spawning, sensitive-env access, writes outside the working tree, cache tampering, toolchain modification, or combined indicators).
---

# Skill: CI test-runtime policy

This skill defines the test-runtime policy the security agent enforces
on PRs. Tests, fixtures, conftests, CI helpers, and workflow steps
must not initiate outbound network connections, spawn processes
outside the test harness, read sensitive environment variables, write
outside the working tree, tamper with cache / persistence mechanisms,
or modify the toolchain in ways that affect other runs.

The recipes below are **starter signals**, not the contract. The
contract is the agent's judgement that the change conforms to the
policy. A motivated author can craft code that evades any specific
regex; the agent is therefore required to read PR-added lines in test
and CI paths with attention to intent — anything that smells off in
the runtime path is in scope, regardless of whether it matches the
patterns below.

This skill is consumed by `security-review.agent.md` (the runtime-policy
category).

---

## Use the recipes as signal, not contract

For every recipe:

1. Run the recipe to surface the obvious cases.
2. Read each hit in its surrounding context and decide whether the
   change conforms to the policy described above the recipe.
3. Independently of the recipes, read PR-added lines in test, fixture,
   conftest, and CI paths and flag anything that violates the policy
   even if no recipe matched it. Examples the recipes alone will miss:
   dynamic / reflective imports, base64- or hex-encoded payloads
   decoded at runtime, indirection through helper modules added in the
   same PR, command construction via string composition from environment
   data, use of unusual primitives (e.g. `ctypes`, `os.urandom` to seed
   a name resolver), code in nonstandard file types loaded by tests.

The agent does NOT need a regex match to raise a finding; the policy
violation is sufficient grounds.

---

## 1. Outbound network connections

```bash
git diff <base>...<head> -- \
    '*test*' '*tests*' '*Test*' '*Tests*' \
    '*fixture*' '*Fixture*' '*setup*' \
    '.github/' 'ci/' '*conftest*' | \
  grep -nE '^\+' | \
  grep -nE 'socket\(|connect\(|getaddrinfo|gethostbyname|requests\.|urllib|urlopen|httpx|aiohttp|fetch\(|curl |wget |nc |netcat |/dev/tcp/|os\.popen|subprocess\.(Popen|run|check_call|check_output|call)|popen|system\(|exec\(|fork\(|http://|https://|ws://|wss://|ftp://'
```

Policy: test code must not initiate outbound network connections.

The agent inspects each hit and decides:

- **Outbound connection to an untrusted or unfamiliar endpoint
  introduced into a test path** → `**must fix**`.
- **Outbound connection to a well-known repository (e.g., `pypi.org`,
  `github.com`, project mirrors) consistent with the existing build
  pattern** → not a finding.
- **Outbound connection introduced where the rest of the test suite
  has no network dependence** → at least `**suggestion**`, with a
  maintainer ping if the legitimacy is unclear.

---

## 2. New subprocess invocations

```bash
git diff <base>...<head> -- '*test*' '*tests*' '*Test*' '*Tests*' \
    '*setup*' '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'subprocess\.|Popen|os\.system|os\.popen|os\.exec|posix_spawn|posixpath\.|shell=True|fork\(|execv|execp|execl|execve|/bin/sh|/bin/bash|sh -c|bash -c'
```

Policy: test code must not spawn processes outside the test harness.

Hits in test code that did not previously invoke subprocesses are at
least `**could fix**`; if the subprocess is invoked with `shell=True`
and the command line is composed from PR-controlled or external
inputs, it is `**must fix**`.

---

## 3. Sensitive environment variable access

```bash
git diff <base>...<head> -- '*test*' '*tests*' '*Test*' '*Tests*' \
    '*setup*' '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'os\.environ|getenv\(|ENV\[|process\.env|System\.getenv|\$\{?(GITHUB_TOKEN|GH_TOKEN|NPM_TOKEN|PYPI|AWS_|GCP_|AZURE_|SECRET|TOKEN|PASSWORD|API_KEY|PRIVATE_KEY|SSH_)'
```

Policy: test code must not read sensitive environment variables
(tokens, secrets, credentials).

Reading `GITHUB_TOKEN` in a path that also has outbound network
activity is a strong signal of unauthorized secret access →
`**must fix**`. Reading other sensitive env vars without obvious
legitimate purpose → at least `**suggestion**` with a maintainer
ping.

---

## 4. Writes outside the working tree

```bash
git diff <base>...<head> -- '*test*' '*tests*' '*Test*' '*Tests*' \
    '*setup*' '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'open\([^,]*[\"'"'"']/(etc|home|root|tmp|var|usr|opt|boot|sys|dev|proc)/|>/etc/|>/home/|>>/etc/|>>/home/|chown |chmod 7|/usr/bin/install |mv /|cp /|symlink |os\.rename\([^,]*[\"'"'"']/'
```

Policy: test code must not write outside the working tree.

Tests that write to `/etc`, `/usr`, `/home`, or modify ownership /
permissions outside the working tree are at least `**suggestion**`;
writing to runner-shared paths (`/tmp/<known cache>`, the GHA cache
directory, the artifact directory) requires verification and is at
least `**could fix**`.

---

## 5. Cache / persistence tampering

```bash
git diff <base>...<head> -- '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'actions/cache|setup-buildx-action|cache-from|cache-to|GITHUB_TOKEN|secrets\.|env:|GITHUB_PATH|GITHUB_ENV|::set-(env|output)|::add-path|core\.exportVariable|core\.addPath'
```

Policy: test code must not tamper with cache, artifact, or
cross-run persistence mechanisms in ways influenced by PR content.

New uses of cache actions or persistence mechanisms in combination
with PR-controllable code → `**must fix**` if the cache key is
influenced by PR content; otherwise at least `**could fix**`.

`::set-env`, `::set-output`, `::add-path` are deprecated workflow
commands and indicate untrusted command injection; their appearance
in a PR-added workflow is `**must fix**`.

---

## 6. Toolchain / install script modification

```bash
git diff <base>...<head> -- 'requirements*.txt' 'pyproject.toml' \
    'setup.py' 'setup.cfg' 'package.json' 'package-lock.json' \
    'pnpm-lock.yaml' 'yarn.lock' '.github/workflows/' \
    '.github/actions/' '.github/scripts/' \
    'Dockerfile*' 'docker-compose*' | \
  grep -nE '^\+'
```

Policy: toolchain modifications that touch secrets or network egress
fall under this policy in addition to supply-chain review.

Any `+` line here is supply-chain-reviewer territory primarily; the
security agent additionally reviews for runtime-policy contributions:

- New downloads of remote scripts piped to a shell:
  `curl … | sh`, `wget … | bash` → `**must fix**` unless the source
  is hash-pinned and provably trustworthy.
- New installs of unfamiliar packages → flagged to the supply-chain
  reviewer; the security agent contributes a `**must fix**` if a
  package install path also touches secrets or network egress in
  combination.

---

## 7. Composite indicator — multiple primitives in the same PR

If two or more of §1, §3, §5 are true on PR-added lines, the
combination is a strong indicator of an attempted runner compromise.
The agent emits a runtime-policy `**must fix**` with body:

```
[Security] **must fix** Multiple runtime-policy indicators introduced into the
CI path: <list the matches>. The combination resembles an attempted
runner compromise (network egress + sensitive-env read + persistence).
The agent recommends not running CI on this PR until manually
reviewed.

cc @<maintainers> — multi-indicator CI risk; please confirm.
```

This is the case that most strongly forces `CI safety: No-Go` AND
the aggregator's `Recommend: Close` heuristic (review-summary
agent §5e trigger 2).

---

## 8. Evasion mindset

The recipes match the *common* shapes. An author who wants to slip
past them has many low-effort options:

- Encode the payload (base64, hex, rot13) and decode at runtime.
- Import the offending module dynamically (`importlib`, `__import__`,
  `getattr` chain).
- Compose the command via string concatenation from values not in the
  same diff line.
- Hide the call behind a helper added in the same PR with an
  innocuous name (`utils.sanity_check()` that resolves a hostname).
- Use uncommon primitives (`ctypes` to call `socket(2)` directly,
  reading `/proc/self/environ` instead of `os.environ`).
- Put the payload in a file type not covered by the recipe globs
  (`.cfg`, `.json` consumed by a test that `eval`s it).

The agent therefore reads the PR-added lines in test and CI paths
for *intent*, not pattern. If a function added in a test path
constructs and executes a command, opens a network primitive, or
reads from a path outside the working tree — regardless of how it
phrases the call — it is in scope. When in doubt the agent posts
at the appropriate severity with a maintainer ping per the
low-confidence rubric in the review contract.

---

## 9. One-line summary

`Scan PR-added lines in test / CI / setup paths for runtime-policy
violations (outbound connections, subprocess spawning, sensitive-env
reads, writes outside the working tree, cache or toolchain tampering)
and combined indicators. Greps are starter signals; the policy is the
contract. Tag at severity; pile-up forces must-fix + maintainer ping.`
