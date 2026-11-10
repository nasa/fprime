# Develop Components in Python

This guide shows how to implement F Prime components in Python and run a deployment that contains them.

[`fprime-python`](https://github.com/fprime-community/fprime-python) exposes F Prime and its standard components as a Python extension module. It autocodes the bindings between a component's C++ base class and an implementation written in Python, so that the rest of the system calls the Python component like any other component.

For a complete project, see the [F Prime Python Reference](https://github.com/fprime-community/fprime-python-reference).

---

## Contents

- [When to Use This](#when-to-use-this)
- [Why Python](#why-python)
- [Prerequisites](#prerequisites)
- [Python in F Prime](#python-in-f-prime)
- [Phased Deployments](#phased-deployments)
- [Project Setup](#project-setup)
- [Create the Component](#create-the-component)
- [Set Up the Deployment](#set-up-the-deployment)
- [Build and Run](#build-and-run)
- [Passing Data Between Python and F Prime](#passing-data-between-python-and-f-prime)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)
- [References](#references)

## When to Use This

Use this guide when you:

- Have existing Python code that you wish to integrate into an F Prime project
- Want to explore F Prime development through Python
- Need to rapidly prototype components or utilities without C++ overhead

## Why Python

Reasons to implement a component in Python rather than C++:

1. Implementing in Python allows the use of existing Python libraries in your code without rewriting the libraries.
2. Python is more approachable to students and researchers who are used to using Python.

Prefer C++ for a component that must meet timing deadlines, must run without dynamic memory allocation, or is critical to the safety of the mission. A common path is to prototype a component in Python and translate it to C++ once its design has settled; the FPP model does not change.

> [!CAUTION]
> `fprime-python` is not flight-grade in the traditional sense: Python uses garbage collection, adds runtime overhead, and is not deterministic. Evaluate with care before electing to use `fprime-python` on a flight system.

## Prerequisites

- A working F Prime development environment and an F Prime project
- Python 3.10 or newer, in the virtual environment used to build the project
- Linux x86_64, macOS arm64, or Windows x64. `fprime-python` reads the FPP model with [`fprime-fpp-python`](https://pypi.org/project/fprime-fpp-python/), which publishes wheels for these platforms only.
- Basic familiarity with FPP, topologies, and Python development

## Python in F Prime

`fprime-python` builds the deployment into a Python extension module named `fprime_py`. A Python interpreter imports that module, sets up the topology, and runs the flight software: the deployment's entry point is a Python file, not a C++ `main`. Components written in C++ run unchanged beside those written in Python.

For each component marked for Python, the build generates bindings that forward the component's handlers (commands, input ports, internal ports) to a Python class, and give that class the component's output ports, events, telemetry channels, and parameters. Modeled types (enums, structs, arrays, aliases) are available in Python under `fprime_py`, e.g. `fprime_py.Fw.CmdResponse`.

![F Prime Python Architecture](https://raw.githubusercontent.com/fprime-community/fprime-python/refs/heads/main/docs/fprime-python-architecture.png)

A Python component may not use the following. Each is rejected at build time with an error naming the offender:

- Serial ports
- State machine instances
- Data product containers

## Phased Deployments

A deployment must configure its components before starting them, e.g. give a rate group its context or a driver its address. A standard deployment does so in hand-written C++, in `Top/<YOUR_DEPLOYMENT_NAME>Topology.cpp`, which calls the autocoded steps one at a time and configures components between them.

A phased deployment has no such file. Each instance carries its configuration code in `Top/instances.fpp` as *phases*, and FPP generates complete `setup` and `teardown` functions from them:

```fpp
instance rateGroupDriver: Svc.RateGroupDriver base id 0x10011000 \
{
    phase Fpp.ToCpp.Phases.configComponents """
    rateGroupDriver.configure(ConfigObjects::MyDeployment_rateGroupDriver::rateGroupDivisorsSet);
    """
}
```

Python deployments use the phased pattern because Python starts the deployment by calling those two generated functions. Code in a hand-written `Topology.cpp` is not bound to Python and would not run. Phases are described in the [FPP User's Guide](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Component-Instances_Init-Specifiers).

To create a phased deployment, run the following from the root of the project and answer the prompts:

```bash
fprime-util new --deployment --phased
```

To convert an existing standard deployment, either create a phased deployment and move the instances and connections into it, or convert in place:

1. For each block of configuration code in `Top/<YOUR_DEPLOYMENT_NAME>Topology.cpp`, add a `phase` holding that code to the instance it configures in `Top/instances.fpp`.
2. Delete `Top/<YOUR_DEPLOYMENT_NAME>Topology.cpp` and `.hpp`, and remove them from `Top/CMakeLists.txt`.

Other deployments in the project need no change. A deployment without Python components keeps running from C++ and builds beside the Python deployment; do not comment it out. A build supports one Python deployment and any number of C++ deployments.

## Project Setup

1. Add `fprime-python` to the project as a submodule:

    ```bash
    git submodule add https://github.com/fprime-community/fprime-python.git lib/fprime-python
    ```

2. List it as an F Prime library in the project's `settings.ini`. Separate multiple libraries with `:`.

    ```ini
    [fprime]
    library_locations: ./lib/fprime-python
    ```

3. Install it into the project's virtual environment. This installs the binding autocoder with its dependencies, [`pybind11`](https://pybind11.readthedocs.io/en/stable/), `fprime-fpp-python`, and `fprime-cpp-codegen`:

    ```bash
    pip install ./lib/fprime-python
    ```

    > [!TIP]
    > Add `./lib/fprime-python/` as a line of the project's `requirements.txt` to install it with the project's other requirements.

4. In the project's top-level `CMakeLists.txt`, include `FprimePython` after `FPrime.cmake` and before `fprime_setup_included_code()`:

    ```cmake
    include("${CMAKE_CURRENT_LIST_DIR}/lib/fprime/cmake/FPrime.cmake")
    include(FprimePython)
    fprime_setup_included_code()
    ```

    This registers the `target/fprime_python` build target that generates and builds the bindings, finds the installed `pybind11`, and compiles the project as position independent code.

5. Create the phased deployment that will hold the Python components, or convert an existing one, as described in [Phased Deployments](#phased-deployments).

6. Generate the build cache:

    ```bash
    fprime-util generate
    ```

## Create the Component

A Python component consists of an FPP model, a Python implementation, and a `CMakeLists.txt`. It has no C++ implementation files. Create it in either of two ways.

**From the template (recommended).** From the directory that will hold the component, run:

```bash
fprime-util new --from-source <path to project>/lib/fprime-python/cookiecutters/component
```

Answer the prompts and add the component to the build when asked. The new component is complete: its model is the one shown [below](#model-the-component), with the name you chose. Continue at [Set Up the Deployment](#set-up-the-deployment), and return to the following sections when changing the component.

**From a C++ component.** Run `fprime-util new --component`, then:

1. Delete the generated `<Component>.cpp` and `<Component>.hpp` files from disk.
2. Remove both files from the `SOURCES` block of the component's `CMakeLists.txt`.
3. Annotate the model, implement it, and register the Python file as described below.

### Model the Component

Annotate the component with `@ fprime-python` on its own line directly above the component definition. Everything else is modeled as for a C++ component, including the standard ports that commands, events, telemetry, and parameters require.

```fpp
module Components {
    @ Echoes a command string
    @ fprime-python
    active component PythonEchoer {
        @ Reports the greeting it is sent as a Hello event
        async command HELLO(
            greeting: string size 40 @< Greeting to report
        )

        @ The greeting received by the HELLO command
        event Hello(
            greeting: string size 40 @< Greeting received
        ) severity activity high format "Hello: {}"

        @ Count of HELLO commands received
        telemetry HelloCount: U32

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @ Port to set the value of a parameter
        param set port prmSetOut
    }
}
```

### Implement the Component

1. Build the project:

    ```bash
    fprime-util build
    ```

    Every build writes `<Component>.template.py` into the component's directory, with a stub for each handler in the model. It replaces `fprime-util impl`.

    > [!TIP]
    > If the file does not appear in VS Code's explorer after the build, refresh the explorer or restart VS Code.

2. If the component has no `<Component>.py` yet, copy the template to that name:

    ```bash
    cp PythonEchoer.template.py PythonEchoer.py
    ```

    After a later change to the model, copy/paste the stubs of the new handlers from the template into `<Component>.py` instead.

3. Implement the handlers:

    ```python
    import fprime_py
    from PythonEchoerBaseAc import PythonEchoerBase


    class PythonEchoer(PythonEchoerBase):
        """ Python implementation for the PythonEchoer component """
        def __init__(self):
            super().__init__()
            self.hello_count = 0

        def HELLO_cmdHandler(self, opCode, cmdSeq, greeting):
            """ Handle the HELLO command """
            self.hello_count += 1
            self.log_ACTIVITY_HI_Hello(greeting)
            self.tlmWrite_HelloCount(self.hello_count)
            self.cmdResponse_out(opCode, cmdSeq, fprime_py.Fw.CmdResponse(fprime_py.Fw.CmdResponse.T.OK))
    ```

    > [!CAUTION]
    > `__init__` runs while the topology initializes its components, before any port is connected. Initialize Python state there, but do not send events, write telemetry, or call ports.

4. List `<Component>.py` in the `SOURCES` block of the component's `CMakeLists.txt`. No C++ file belongs there:

    ```cmake
    register_fprime_library(
        AUTOCODER_INPUTS
            "${CMAKE_CURRENT_LIST_DIR}/PythonEchoer.fpp"
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/PythonEchoer.py"
    )
    ```

## Set Up the Deployment

The steps below use `<YOUR_DEPLOYMENT_NAME>` for the name of the phased deployment. Replace it, including inside file names.

1. **Instantiate the component.** A Python component is instantiated and connected like any other. Define the instance in `Top/instances.fpp`:

    ```fpp
    instance pythonEchoer: Components.PythonEchoer base id 0x10005000 \
      queue size Default.QUEUE_SIZE \
      stack size Default.STACK_SIZE \
      priority 39
    ```

    Then add it to the topology in `Top/topology.fpp`. Without this line the component is not part of the deployment:

    ```fpp
    instance pythonEchoer
    ```

2. **Annotate the topology.** In `Top/topology.fpp`, add `@ fprime-python` directly above the deployment topology. This is what makes the deployment a Python deployment: the build binds its `setup` and `teardown` and its Python instances. A topology that instantiates Python components without the annotation is rejected.

    ```fpp
    @ fprime-python
    deployment topology <YOUR_DEPLOYMENT_NAME> {
    ```

3. **Drive the rate groups from Python.** In a C++ deployment, `main` blocks in the timer component that cycles the rate groups. Python cannot call that timer, since only Python components are bound. In `Top/instances.fpp`, replace the timer's component with `FprimePython.PythonRateGroupDriver`, a Python component with the same output port:

    ```fpp
    instance timer: FprimePython.PythonRateGroupDriver base id 0x10013000
    ```

    The connection `timer.CycleOut -> rateGroupDriver.CycleIn` in `Top/topology.fpp` stays as it is.

4. **Own the hostname.** In `Top/<YOUR_DEPLOYMENT_NAME>TopologyDefs.hpp`, the phased template declares `TopologyState::hostname` as `const char*`. Python cannot safely assign to such a member: it is left pointing into a Python string that is freed after the assignment. Declare it as a `std::string` and add `#include <string>`:

    ```cpp
    std::string hostname;  //!< Hostname for TCP communication
    ```

    Update its uses in the `comDriver` phases of `Top/instances.fpp`: replace both `state.hostname != nullptr` with `!state.hostname.empty()`, and pass `state.hostname.c_str()` to `comDriver.configure`.

5. **Bind `TopologyState`.** `TopologyState` is hand-written C++, not part of the FPP model, so it is not bound automatically; yet Python must construct one to pass to `setup`. Create `Top/<YOUR_DEPLOYMENT_NAME>Bindings.cpp` defining `setup_user_deployment`, which `fprime_py` calls when it is imported:

    ```cpp
    #include "FprimePython/FprimePython.hpp"
    #include "<YOUR_DEPLOYMENT_NAME>/Top/<YOUR_DEPLOYMENT_NAME>TopologyAc.hpp"
    #include "<YOUR_DEPLOYMENT_NAME>/Top/<YOUR_DEPLOYMENT_NAME>TopologyDefs.hpp"

    void setup_user_deployment(pybind11::module_& m) {
        // See: https://pybind11.readthedocs.io/en/stable/classes.html
        pybind11::class_<<YOUR_DEPLOYMENT_NAME>::TopologyState>(m, "TopologyState")
            .def(pybind11::init<>())
            .def_readwrite("hostname", &<YOUR_DEPLOYMENT_NAME>::TopologyState::hostname)
            .def_readwrite("port", &<YOUR_DEPLOYMENT_NAME>::TopologyState::port);

        // The phased template does not name the parameter file, which setup requires
        FileHandling::prmDb.configure("PrmDb.dat");
    }
    ```

    Bind the members your `TopologyState` has; `hostname` and `port` are those of the phased template. The include paths start at the project root, so prefix them with any directories between the root and the deployment.

6. **Register the bindings.** Add the file to `Top/CMakeLists.txt` in a `SOURCES` block:

    ```cmake
    register_fprime_module(
        AUTOCODER_INPUTS
            "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
            "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/<YOUR_DEPLOYMENT_NAME>Bindings.cpp"
        ...
    )
    ```

7. **Write the entry point.** Create `fsw_main.py` in the deployment's directory. It must define a function `fsw_main()` taking no arguments:

    ```python
    """ Starts the <YOUR_DEPLOYMENT_NAME> flight software from Python """
    import argparse
    import signal

    import fprime_py


    def parse_args():
        """ Parse the address the flight software listens on for the GDS """
        parser = argparse.ArgumentParser(description="<YOUR_DEPLOYMENT_NAME> flight software")
        parser.add_argument("--hostname", type=str, default="127.0.0.1", help="Address to listen on")
        parser.add_argument("--port", type=int, default=50000, help="Port to listen on")
        return parser.parse_args()


    def handle_sigterm(signum, frame):
        """ Shut down on SIGTERM as on CTRL-C """
        raise KeyboardInterrupt


    def fsw_main():
        """ Entry point: fprime-python-runner requires this name and no arguments """
        args = parse_args()
        signal.signal(signal.SIGTERM, handle_sigterm)
        topology_state = fprime_py.TopologyState()
        topology_state.hostname = args.hostname
        topology_state.port = args.port
        fprime_py.Os.init()
        try:
            fprime_py.<YOUR_DEPLOYMENT_NAME>.setup(topology_state)
            # Blocks, driving the rate groups once a second, until CTRL-C
            fprime_py.<YOUR_DEPLOYMENT_NAME>.Instances.timer.driveRateGroup(fprime_py.Fw.TimeInterval(1, 0))
        except KeyboardInterrupt:
            print("[INFO] CTRL-C received, shutting down")
        fprime_py.<YOUR_DEPLOYMENT_NAME>.teardown(topology_state)


    if __name__ == "__main__":
        fsw_main()
    ```

    - `--hostname` and `--port` reach the communication driver through `TopologyState`: Python assigns them to the bound members, `setup` receives the state, and the `comDriver` phases read `state.hostname` and `state.port`. The default address accepts a GDS on the same machine only; pass `--hostname 0.0.0.0` to listen on every network interface.
    - `Instances` holds the Python component instances of the topology by instance name. `Instances.timer` exists because `timer` is now a `PythonRateGroupDriver`; the call is required, as nothing else cycles the rate groups. The interval must match the base rate the rate group divisors assume.
    - The [reference deployment's entry point](https://github.com/fprime-community/fprime-python-reference/blob/main/FprimePythonReference/ReferenceDeployment/fsw_main.py) also calls `Instances.pythonCom`, a Python communication component of that deployment. Omit those calls unless your topology has such an instance.

8. **Stub the C++ `main`.** The build still produces a deployment executable, but nothing runs it. Replace the contents of the deployment's `Main.cpp` with:

    ```cpp
    int main(int argc, char* argv[]) {
        return 0;
    }
    ```

9. **Register the entry point.** In the deployment's `CMakeLists.txt`, add `fsw_main.py` to `SOURCES`, and choose the Python console so that F Prime console output is printed by Python:

    ```cmake
    register_fprime_deployment(
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/fsw_main.py"
        CHOOSES_IMPLEMENTATIONS
            FprimePython_PythonConsole
        DEPENDS
            ${FPRIME_CURRENT_MODULE}_Top
    )
    ```

    The build links the deployment executable against `pybind11::embed`, which its Python components need. Do not add it yourself.

## Build and Run

1. Build from the root of the project or from the deployment's directory:

    ```bash
    fprime-util build
    ```

    The build installs `fprime_py`, `fsw_main.py`, and every Python component to `build-artifacts/python`. Python files are reinstalled by every such build, so after editing only Python, `fprime-util build` is quick. A build from a component's directory builds that component alone and installs nothing.

2. Start the flight software with the Python interpreter that built it. It is a separate process from the GDS:

    ```bash
    python3 build-artifacts/python/fsw_main.py --hostname 127.0.0.1 --port 50000
    ```

    `fprime-python-runner` does the same from any directory of the project, without arguments.

3. In a second terminal, start the GDS without an application (`-n`), since the flight software is already running. The phased template's `comDriver` is a TCP server, so the GDS connects as a client:

    ```bash
    fprime-gds -n --ip-client --ip-address 127.0.0.1 --ip-port 50000 \
        --dictionary build-artifacts/<platform>/<YOUR_DEPLOYMENT_NAME>/dict/<YOUR_DEPLOYMENT_NAME>TopologyDictionary.json
    ```

4. Send `<YOUR_DEPLOYMENT_NAME>.pythonEchoer.HELLO` from the GDS. The `Hello` event and `HelloCount` channel report the result.

The flight software and the GDS may be started in either order.

Stop the flight software with CTRL-C.

## Passing Data Between Python and F Prime

- Enums are passed as `<Enum>.T` values and wrapped to construct an F Prime enum: `fprime_py.Fw.CmdResponse(fprime_py.Fw.CmdResponse.T.OK)`.
- A Python `str` passed to F Prime must fit the string it is copied into, counted in UTF-8 bytes, and must not contain NUL characters. A string that does not fit raises `ValueError`; it is never silently truncated. With the default configuration an event string argument holds at most 200 bytes and a telemetry string 40 bytes, even when declared larger.
- An exception that leaves a handler is not caught by F Prime: on a component's thread it ends the flight software. Handle the exceptions a handler can raise inside the handler, and check the length of strings from untrusted sources before sending them.

The [`fprime-python` README](https://github.com/fprime-community/fprime-python/blob/main/README.md) lists the Python name and signature of every kind of handler and the limits of each kind of string.

## Testing

Unit testing of Python components is not yet supported. Test at the system level with the [GDS integration test API](../../user-manual/gds/gds-test-api-guide.md): with the flight software and the GDS running as [above](#build-and-run), run:

```bash
pytest <test directory> --dictionary <path to dictionary>
```

## Troubleshooting

| Symptom | Cause |
| --- | --- |
| `ModuleNotFoundError: No module named 'fprime_py'` | The build has no Python deployment: no deployment topology is annotated `@ fprime-python`. Also raised when running with a different Python than built the project. |
| `undefined symbol: ...setup_user_deployment...` on import | The bindings file is missing from the `SOURCES` of `Top/CMakeLists.txt`. |
| Build error that a topology has Python components but no annotation | Add `@ fprime-python` above the deployment topology. |
| The flight software aborts during `setup` | The parameter file is not named: call `FileHandling::prmDb.configure` as shown in the bindings step. |
| The hostname is garbled or the GDS never connects | `TopologyState::hostname` is still a `const char*`. |
| `Could not find a package configuration file provided by "pybind11"` or a `pybind11 --cmakedir` failure | `fprime-python` is not installed in the active virtual environment. |

## References

- [F Prime Python](https://github.com/fprime-community/fprime-python)
- [F Prime Python Reference](https://github.com/fprime-community/fprime-python-reference)
- [Develop Subtopologies: phases](./develop-subtopologies.md#unified-fpp-with-phases)
- [Pybind11](https://pybind11.readthedocs.io/en/stable/)
- [Pybind11 - Classes](https://pybind11.readthedocs.io/en/stable/classes.html)
