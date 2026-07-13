# Execution Time Annotations (Prototype)

This document describes a **prototype convention** for modeling worst-case execution time
(WCET) and handler deadlines in FPP. It responds to [fprime#3688](https://github.com/nasa/fprime/issues/3688)
and is intended for external experimentation before any framework enforcement lands.

## Goals

- Let projects document per-port timing budgets in the model itself.
- Surface those budgets in machine-readable FPP outputs (`fpp-to-json` annotations).
- Enable offline reports and future platform-specific enforcement hooks.

Enforcement (runtime measurement, deadline exceptions, compile-time checks) is **out of scope**
for this prototype and remains project-specific.

## Proposed modeling convention

Use FPP `@` annotations on ports to declare a timing budget. Suggested forms:

```fpp
active component ControlLoop {
    @ WCET 200 us
    sync input port schedIn: Svc.Sched

    @ deadline 50 us
    async input port transmit: Fw.Buffer

    @ WCET 10 us
    output port loadDmaEngine: Fw.Buffer
}
```

Projects may also use free-form annotations that include a numeric duration token:

```fpp
@ Execution time limit: 200us for rate group member
sync input port schedIn: Svc.Sched
```

The `fprime-util wcet` command (see `nasa/fprime-tools`) scans `fpp-to-json` AST output for
these annotations and prints a tabular report.

## Where annotations appear today

FPP annotations are already emitted in `fpp-to-json` output under each modeled element's
`annotation` field. See the [FPP JSON dictionary reference](../../../reference/fpp-json-dict.md).

No autocoder changes are required for the reporting prototype: tooling reads the JSON AST.

## Suggested workflow

1. Annotate ports in component `.fpp` files using the convention above.
2. Run `fprime-util fpp-check` / build as usual.
3. Generate AST JSON (build cache or `fpp-to-json` on topology sources).
4. Run `fprime-util wcet --json <path-to-ast.json>` to produce a WCET report.

## Future work

- First-class `duration` syntax in FPP (parser + type checker).
- Autocoder hooks to inject measurement stubs around annotated handlers.
- Optional compile-time warnings when annotated budgets exceed platform limits.

Feedback on this prototype is welcome on [#3688](https://github.com/nasa/fprime/issues/3688).
