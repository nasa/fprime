<h2 align="center">A Flight-Proven, Multi-Platform, Open-Source Flight Software Framework</h2>
<p align="center"><br/>
<img width="200em" src="docs/img/fprime-logo.svg"><br/>
</p>

#
F´ (F Prime) is a component-driven framework that enables rapid development and deployment of spaceflight and other embedded software applications. Originally developed at the [Jet Propulsion Laboratory](https://www.jpl.nasa.gov/), F´ has been successfully deployed on [several space applications](https://fprime.jpl.nasa.gov/overview/projects/). It is tailored but not limited to small-scale spaceflight systems such as CubeSats, SmallSats, and instruments.

**Please Visit the F´ Website:** [https://fprime.jpl.nasa.gov](https://fprime.jpl.nasa.gov/) for more information.


## What does F´ provide

- An architecture that decomposes flight software into discrete components with well-defined interfaces
- A C++ framework that provides core capabilities such as message queues and threads
- Modeling tools for specifying components and connections and automatically generating code
- A growing collection of ready-to-use components
- Testing tools for testing flight software at the unit and integration levels.

Learn more about [F´ key features](https://fprime.jpl.nasa.gov/overview).


## System Requirements

The following system requirements apply to your workstation for developing F´ applications. To see the list of supported platforms on which F´ applications can run on, see [Supported Platforms](./docs/user-manual/framework/supported-platforms.md).

1. Linux, Windows with WSL, or macOS operating system

2. [git](https://git-scm.com/)

3. [Python 3.9+](https://www.python.org/downloads/), [virtual environments](https://docs.python.org/3/library/venv.html), and [PIP](https://pypi.org/project/pip/)

4. [Clang](https://clang.llvm.org/) or [GNU C and C++ compilers](https://gcc.gnu.org/) (e.g. gcc and g++)

<details>
<summary>Click to Expand: How to Verify System Requirements</summary>

2. `git --version`

3. `python --version` and `python -m venv -h`. Your system might use an alternate like `python3` or `python3.13`. For pip: `pip --version`.  On your system it might be `pip3` or `pipx`.

4. `g++  --version` and then create, build, and run a test program.  For example, create a file named `hello.cpp` with contents:

    ```cpp
    #include <iostream>
    int main(void){
        std::cout << "Hello, World!" << std::endl;
    }
    ```

    then build and run it, for example like:
    ```bash
    % g++ -o hi hello.cpp
    % ./hi
    Hello, World!
    ```

</details>


## Getting Started

To get started with F´, install the F´ bootstrapping tool with:
```
pip install fprime-bootstrap
```

Then, create a new project with:
```
fprime-bootstrap project
```

See the [HelloWorld Tutorial](https://fprime.jpl.nasa.gov/latest/tutorials-hello-world/docs/hello-world/) to guide you through all the steps of developing an F´ project.

New users are encouraged to read through the [User Manual](https://fprime.jpl.nasa.gov/latest/docs/user-manual/) and explore the [other tutorials](https://fprime.jpl.nasa.gov/latest/docs/tutorials/).


## Getting Help

### Discussions
To ask questions, discuss improvements, and ask for help please use the project's [GitHub Discussions](https://github.com/nasa/fprime/discussions).

### Bug reports
To report bugs and issues, [open an issue here](https://github.com/nasa/fprime/issues).

### Community
The [F´ Community](https://github.com/fprime-community) GitHub Organization contains third party contributions, more documentation of flight software development, and additional resources.


## Resources
- [User Manual](https://fprime.jpl.nasa.gov/latest/docs/user-manual/)
- [Tutorials](https://fprime.jpl.nasa.gov/latest/docs/tutorials/)
- [Discussions](https://github.com/nasa/fprime/discussions)
- [Submit an Issue](https://github.com/nasa/fprime/issues)
- [F´ Community](https://github.com/fprime-community)

## Meet the Team

The following roles are members of the CCB and per our [governance](./GOVERNANCE.md) document have decision making power within the framework.

| Role              | Team Member     |
|-------------------|-----------------|
| Community Manager | @LeStarch       |
| Community Manager | @thomas-bc      |
| Security Overseer | @bitWarrior     |
| CCB Member        | @bocchino       |
| CCB Member        | @kevin-f-ortega |
| CCB Member        | @SterlingPeet   |
| CCB Member        | @timcanham      |
| CCB Member        | @zimri-leisher  |

Each product has a set of Maintainers who are charged with the day-to-day implementation of the CCB's direction as realized in a by product.

| Product                      | Core Maintainer(s)         |
|------------------------------|----------------------------|
| F Prime                      | @LeStarch, @thomas-bc      |
| F Prime (v3.6.x Maintenance) | @SterlingPeet              |
| fprime-tools                 | @thomas-bc, @LeStarch      |
| fprime-gds                   | @thomas-bc, @LeStarch      |
| fpp                          | @bocchino, @Kronos3        |
| F Prime Platforms            | @kevin-f-ortega, @LeStarch |
| Security                     | @bitWarrior                |

## Release Notes

The version history and artifacts associated with the project can be found at [Releases](https://github.com/nasa/fprime/releases).
