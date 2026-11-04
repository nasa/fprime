# Advanced Sequencing in F´ with Fpy

Fpy is an advanced sequencing language for F´, designed for both software and non-software engineers.

This is a quickstart guide to writing and running a simple Fpy sequence. It should be easy to follow for someone who has used Python and F´ before.

> [!TIP]
> For a more in-depth guide, see the [Fpy User's Guide](https://github.com/fprime-community/fpy/blob/main/README.md)

Install Fpy with:
```sh
$ pip install fprime-fpy[wasm]
Collecting fprime-fpy[wasm]
  Downloading fprime_fpy-0.6.2-py3-none-any.whl.metadata (28 kB)
...
Downloading fprime_fpy-0.6.2-py3-none-any.whl (192 kB)
Installing collected packages: fprime-fpy
Successfully installed fprime-fpy-0.6.2
```

Now we can write our first sequence. Fpy looks a lot like Python:
```py
# hash denotes a comment
log("hello world!")
```

Let's save that sequence as `test.fpy`, and compile it with:
```sh
$ fprime-fpyc test.fpy --emit wasm --dictionary <path to your dictionary>
test.wasm
size 182 B
```

Make sure your deployment has a `Svc.WasmSequencer` instance. Let's assume it's named `wasmSeq`. To run the sequence, send the following command via the GDS:
```
wasmSeq.RUN "<path to test.wasm>", Svc.BlockState.BLOCK
```

You should get an event that says "hello world!":
![An F Prime GDS event saying "hello world!"](./img/fpy-hello-world.png)

That's all it takes to write, compile and run an Fpy sequence!

Fpy has many more features. For a complete guide, see the [Fpy User's Guide](https://github.com/fprime-community/fpy/blob/main/README.md). Here's a brief overview of the features:

### Commands, variables and math
You can call commands:
```py
CdhCore.cmdDisp.CMD_NO_OP()

# you can declare variables and do math:
some_var: F32 = 1 - 2 + 3 * 4 + 10 / 5 * 2
# some_var is a variable of type F32

# you can pass variable arguments to commands;
Ref.exampleComponent.EXAMPLE_CMD(example_arg=some_var)
```
### Telemetry and parameter database access
You can get parameter and telemetry channel values:
```py
param_val: U8 = Ref.sendBuffComp.parameter3
tlm_val: U32 = CdhCore.cmdDisp.CommandsDispatched
```

### Absolute and relative sleep
You can sleep for relative durations:
```py
sleep(seconds=1, useconds=500_000)
# and you can sleep until absolute times:
sleep_until(time("2025-12-19T14:30:00.123456Z"))
```

### If/elif/else and check statements
If statements are supported:
```py
if 123 < 456:
    log("should happen")
else:
    log("uh oh")
```
There's a "check" statement, which waits until its condition is true:
```py
check CdhCore.cmdDisp.CommandsDispatched > 1
    timeout {seconds: 60} # optional timeout
    persist {seconds: 2} # optional duration for which the condition must persist
    period {seconds: 1}: # optional frequency at which to check the condition
    log("more than 30 commands for 2 seconds!")
timeout:
    # the timeout block runs if the check times out
    log("took more than 60 seconds :(")
```

This is useful for taking actions when certain events happen, such as a state transition:
```py
check ExampleGncComponent.STATE == GncState.SUN_POINTING timeout never
# this will wait until the state is SUN_POINTING
```

### Functions, for and while loops
You can define functions:
```py
def new_func(a: U64, b: U64) -> U64:
    return a + b
```

You can do for and while loops:

```py
sum: I64 = 0
# loop i from 0 inclusive to 5 exclusive
for i in 0 .. 5:
    sum = sum + i

assert sum == 10

counter = 0
while True:
    counter = counter + 1
    if counter == 100:
        break
```

### Imports and sequence arguments
You can import functions from other sequences:
```py
# in library.fpy:
def norm(x: F64, y: F64, z: F64) -> F64:
    return (x ** 2 + y ** 2 + z ** 2) ** 0.5
```

```py
from library import norm

assert norm(1, 0, 0) >= 0
```

You can declare sequence arguments, which must be provided when you run the sequence:
```py
# this sequence takes two arguments: a bool and a U32
sequence(arg1: bool, arg2: U32)

if arg1 and arg2 == 123:
    ...
```

There are many more features, see the [user's guide](https://github.com/fprime-community/fpy/blob/main/README.md) for more details.

## Technical details

The [Fpy compiler](https://github.com/fprime-community/fpy) is a multi-pass compiler written in Python. It compiles Fpy programs to Wasm, a simple and safe bytecode, which is then interpreted by a Wasm interpreter in the `Svc.WasmSequencer` component.


Resources:
* [Fpy User's Guide](https://github.com/fprime-community/fpy/blob/main/README.md)
* [Fpy Language Specification](https://github.com/fprime-community/fpy/blob/main/SPEC.md)
* [Overview of Fpy tests](https://github.com/fprime-community/fpy/blob/main/README.md#overview-of-tests)


For more information on `Svc.WasmSequencer`, reference the [Svc.WasmSequencer](../../../Svc/WasmSequencer/docs/sdd.md) SDD documentation. For the system-level description of this capability, see [Advanced Sequencing Functionality](../../reference/system-functional/advanced-sequencing.md).

