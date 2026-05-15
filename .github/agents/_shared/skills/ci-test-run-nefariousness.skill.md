# Skill: CI test-run nefariousness (grep recipes)

The security agent's category 8 — "any code the PR introduces that
behaves nefariously during CI test runs" — is implemented by the
recipes in this skill.

The premise: an attacker who can land code in a PR can run arbitrary
code on the GitHub Actions runner during CI (the test step executes
whatever the PR provides). A high-value attack is to use that
execution time to:

- Exfiltrate secrets accessible to the runner (`GITHUB_TOKEN`,
  org secrets, OIDC tokens).
- Reach outbound to attacker-controlled infrastructure.
- Persist on the cache / artifact / network in ways that affect
  other runs.
- Pivot laterally (write to the host filesystem outside the working
  tree, tamper with shared caches).

This skill is grep recipes for spotting those patterns in the PR
diff. It is consumed by `security-review.agent.md` category 8.

---

## 1. New network egress in test code

```bash
# Network egress primitives appearing on a + line in test / fixture
# / CI helper paths
git diff <base>...<head> -- \
    '*test*' '*tests*' '*Test*' '*Tests*' \
    '*fixture*' '*Fixture*' '*setup*' \
    '.github/' 'ci/' '*conftest*' | \
  grep -nE '^\+' | \
  grep -nE 'socket\(|connect\(|getaddrinfo|gethostbyname|requests\.|urllib|urlopen|httpx|aiohttp|fetch\(|curl |wget |nc |netcat |/dev/tcp/|os\.popen|subprocess\.(Popen|run|check_call|check_output|call)|popen|system\(|exec\(|fork\(|http://|https://|ws://|wss://|ftp://'
```

The agent inspects each hit and decides:

- **Outbound network call to an attacker-controlled or unfamiliar
  endpoint introduced into a test path** → `**must fix**`.
- **Outbound network call to a well-known repository (e.g.,
  `pypi.org`, `github.com`, project mirrors) that is consistent with
  the existing build pattern** → not a finding.
- **Outbound network call introduced where the rest of the test
  suite has no network dependence** → at least `**suggestion**`,
  with a maintainer ping if the legitimacy is unclear.

---

## 2. New subprocess invocations

```bash
git diff <base>...<head> -- '*test*' '*tests*' '*Test*' '*Tests*' \
    '*setup*' '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'subprocess\.|Popen|os\.system|os\.popen|os\.exec|posix_spawn|posixpath\.|shell=True|fork\(|execv|execp|execl|execve|/bin/sh|/bin/bash|sh -c|bash -c'
```

Hits in test code that did not previously invoke subprocesses are at
least `**could fix**`; if the subprocess is invoked with `shell=True`
and the command line is composed from PR-controlled or external
inputs, it is `**must fix**`.

---

## 3. Environment variable harvesting

```bash
git diff <base>...<head> -- '*test*' '*tests*' '*Test*' '*Tests*' \
    '*setup*' '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'os\.environ|getenv\(|ENV\[|process\.env|System\.getenv|\$\{?(GITHUB_TOKEN|GH_TOKEN|NPM_TOKEN|PYPI|AWS_|GCP_|AZURE_|SECRET|TOKEN|PASSWORD|API_KEY|PRIVATE_KEY|SSH_)'
```

Reading `GITHUB_TOKEN` in a path that also has network egress is a
strong signal of secret exfiltration → `**must fix**`. Reading other
env vars without obvious legitimate purpose → at least
`**suggestion**` with a maintainer ping.

---

## 4. Writes outside the working tree

```bash
git diff <base>...<head> -- '*test*' '*tests*' '*Test*' '*Tests*' \
    '*setup*' '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'open\([^,]*[\"'"'"']/(etc|home|root|tmp|var|usr|opt|boot|sys|dev|proc)/|>/etc/|>/home/|>>/etc/|>>/home/|chown |chmod 7|/usr/bin/install |mv /|cp /|symlink |os\.rename\([^,]*[\"'"'"']/'
```

Tests that write to `/etc`, `/usr`, `/home`, or modify ownership /
permissions outside the working tree are at least `**suggestion**`;
writing to runner-shared paths (`/tmp/<known cache>`, the GHA cache
directory, the artifact directory) requires verification and is at
least `**could fix**`.

---

## 5. CI persistence / cache tampering

```bash
git diff <base>...<head> -- '.github/' 'ci/' | \
  grep -nE '^\+' | \
  grep -nE 'actions/cache|setup-buildx-action|cache-from|cache-to|GITHUB_TOKEN|secrets\.|env:|GITHUB_PATH|GITHUB_ENV|::set-(env|output)|::add-path|core\.exportVariable|core\.addPath'
```

New uses of cache actions or GHA persistence mechanisms in
combination with PR-controllable code → `**must fix**` if the cache
key is influenced by PR content; otherwise at least `**could fix**`.

`::set-env`, `::set-output`, `::add-path` are deprecated and
indicate untrusted commands; their appearance in a PR-added workflow
is `**must fix**`.

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

Any `+` line here is supply-chain-agent territory primarily, but
the security agent also reviews for nefariousness:

- New downloads of remote scripts piped to a shell:
  `curl … | sh`, `wget … | bash` → `**must fix**` unless the source
  is hash-pinned and provably trustworthy.
- New installs of unfamiliar packages → flagged to the supply-chain
  agent; the security agent contributes a `**must fix**` if a
  package install path also touches secrets / network egress in
  combination.

---

## 7. Auto-generated / vendored code that diffs without an input change

```bash
# Files commonly generated; flag if they diff but no upstream input
# diffs alongside them
for path in 'gen/' 'autocoder-generated/' 'build*/' 'cmake-build*/' ; do
  diff_count=$(git diff <base>...<head> --name-only -- "$path" | wc -l)
  if [ "$diff_count" -gt 0 ]; then
    echo "$path changed: $diff_count files"
  fi
done
```

If the diff includes generated code but no FPP / configuration /
generator-input change explains it, the generated content may have
been hand-edited (which is forbidden) or generated against a
modified generator (which is supply-chain territory). At minimum
`**suggestion**` with a maintainer ping.

---

## 8. Composite check — multiple primitives in the same PR

If two or more of §1, §3, §5 are true on PR-added lines, the
combination is a strong signal of an attempted CI compromise. The
agent emits a category-8 `**must fix**` with body:

```
**must fix** Multiple suspicious primitives introduced into the CI
path: <list the matches>. The combination resembles an attempted
runner compromise (network egress + secret read + persistence). The
agent recommends not running CI on this PR until manually reviewed.

cc @<maintainers> — multi-primitive CI risk; please confirm.
```

This is the case that most strongly forces `CI safety: No-Go` AND
the aggregator's `Recommend: Close` heuristic (review-summary
agent §5e trigger 2).

---

## 9. One-line summary

`grep PR-added lines in test/CI/setup paths for network primitives,
subprocess invocations, env-var harvesting, writes outside the
working tree, cache/persistence mechanisms, and combined primitives.
Tag at severity; pile-up forces must-fix + maintainer ping.`
