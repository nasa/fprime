# F Prime — guide for AI agents

F Prime (F´) is a component-driven framework for flight software and embedded
systems: components are modeled in [FPP](https://nasa.github.io/fpp/), the build
system autocodes C++ base classes from those models, and developers implement the
generated handlers.

This file is a router. It tells you where things are and which procedure to
follow; the details live in the documents linked below.

## Repository map

| Path | Contents |
| --- | --- |
| `Fw/` | Core framework: types, serialization, ports, component base classes, command/event/telemetry/parameter infrastructure. |
| `Svc/` | Standard service components (command dispatch, telemetry, sequencers, file handling, health, data products). |
| `Drv/` | Hardware drivers (UART, I2C, SPI, GPIO, IP sockets). |
| `Os/` | OS abstraction layer (OSAL) with per-platform implementations (POSIX, Linux, Darwin, baremetal). |
| `Utils/`, `CFDP/`, `STest/`, `TestUtils/` | Support libraries: hashing, CRC, rate limiting, CFDP checksums, randomized/rule-based test infrastructure. |
| `cmake/` | Build system: CMake API, targets, toolchains, platforms, autocoder integration. See `cmake/AGENTS.md`. |
| `docs/` | User-facing documentation website sources. See `docs/AGENTS.md`. |
| `default/config/` | Default compile-time configuration headers and `AcConstants.fpp`; projects override these to tune a deployment. |
| `TestDeploymentsProject/Ref/` | The `Ref` reference deployment, used to exercise the framework end to end. |
| `FppTestProject/` | Validation suite for FPP-generated code. |
| `.github/agents/`, `.github/skills/` | Agent definitions and reusable agent procedures. |

For a narrative tour of the tree, see
[`docs/user-manual/overview/source-tree.md`](docs/user-manual/overview/source-tree.md).

## Anatomy of a component

A component directory (e.g. `Svc/CmdDispatcher/`) contains:

- `<Name>.fpp` — the model: ports, commands, events, telemetry channels,
  parameters. **The model is the source of truth; change it first.**
- `<Name>.hpp` / `<Name>.cpp` — the implementation of the handlers declared by
  the model.
- `docs/sdd.md` — the Software Design Document describing the component's
  interface and behavior. Keep it in sync with the model.
- `test/ut/` — unit tests built on the autocoded `TesterBase`/`GTestBase`.
- `CMakeLists.txt` — registers sources, the model, and the unit tests.

Files ending in `Ac.hpp` / `Ac.cpp` are autocoded build products under
`build-fprime-*/`. Never edit them; change the `.fpp` model and rebuild.

## Commands

```bash
source fprime-venv/bin/activate     # provides fprime-util, cmake, ctest, clang-format

fprime-util generate                # generate the build cache
fprime-util build -j"$(nproc)"      # build

fprime-util generate --ut            # generate the unit-test build cache
fprime-util check -j"$(nproc)"       # build and run unit tests (from a module directory or the root)

clang-format -i <files>             # formatting; .clang-format lives at the repository root
```

Deployments build from their own directory, e.g.
`cd TestDeploymentsProject/Ref && fprime-util generate && fprime-util build`.

Build-system usage, `fprime-util` subcommands, and settings are documented in
[`docs/user-manual/build-system/`](docs/user-manual/build-system/).

## Conventions to respect

- **C++14**, no exceptions, no RTTI, no dynamic allocation after
  initialization. Use F Prime types (`Fw::Buffer`, `Fw::String`, the
  fixed-width `Fw` numeric types) and `FW_ASSERT` rather than standard-library
  equivalents.
- Flight code must be deterministic and bounded: no unbounded loops, no
  unbounded buffers, explicit handling of every error return.
- The authoritative C/C++ rules are in
  [`.github/skills/fprime-cpp-design/SKILL.md`](.github/skills/fprime-cpp-design/SKILL.md);
  JPL-level design expectations are in
  [`.github/skills/jpl-design-principles/SKILL.md`](.github/skills/jpl-design-principles/SKILL.md).

## Procedures (skills)

Reusable procedures live in `.github/skills/<skill-name>/SKILL.md` and follow the
[Agent Skills specification](https://agentskills.io/specification). Follow the
relevant one instead of improvising:

- Building a component end to end: `fprime-component-development`, which
  sequences `fprime-component-requirements`,
  `fprime-component-design-fpp`, `fprime-component-implementation`,
  `fprime-component-unit-test`, `fprime-component-integration-test`.
- Writing unit tests: `fprime-unit-testing`.
- Driving development to convergence through local test-and-review
  iterations: `fprime-iterative-development` (requires explicit user
  approval before use).
- C/C++ design rules: `fprime-cpp-design`.
- Tracing untrusted inputs: `fprime-ground-input-tracing`,
  `fprime-hardware-input-tracing`.
- Writing a subsystem reference page: `write-system-functional-doc`.
- Adding or changing a skill or agent: `agent-skill-authoring`.

Pull-request review is performed by the multi-agent flow in
[`.github/agents/`](.github/agents/), entered through
`review-orchestrator.agent.md`. Development work (components,
topologies, build system) is entered through
`fprime-development.agent.md`, which sequences the skills above.

## Contributing

- Contributions start with an **issue**, approved by the Change Control Board,
  before implementation. Keep changes focused; large cross-cutting PRs are
  declined. See [`CONTRIBUTING.md`](CONTRIBUTING.md) and
  [`GOVERNANCE.md`](GOVERNANCE.md).
- **Generative-AI usage must be disclosed** in the pull request, per
  [`AI_POLICY.md`](AI_POLICY.md): fill in the "AI Used (y/n)" entry and describe
  the assistance in the "AI Usage" section of the PR template.
- Update the documentation surfaces a change invalidates — component `docs/sdd.md`
  first, then the pages under `docs/` (see `docs/AGENTS.md`).
