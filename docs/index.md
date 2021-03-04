---
title: "F´ A Flight Software and Embedded Systems Framework"
layout: default
---
F´ is a software framework for rapid development and deployment of embedded systems and spaceflight applications.
Originally developed at NASA's Jet Propulsion Laboratory, F´ is open source software that has been successfully deployed
for several space applications. It has been used for, but is not limited to, CubeSats, SmallSats, instruments, and
deployables.

F´ has the following features:
* Component architecture with well-defined interfaces
* C++ framework providing core capabilities like queues, threads, and operating-system abstraction
* Tools for designing systems and automatically generating code from systems design
* A standard library of flight-worthy components
* Testing tools for unit and system-level testing

| F´ Resources | |
|---|---|
| Features | [Features](./features.md) |
| Projects  | [Projects](./projects.md) |
| Installation | [INSTALL.md](./INSTALL.md) | 
| Tutorials | [Tutorials](./Tutorials/README.md) |
| User Guide | [User Guide](./UsersGuide/guide.md) |
| Repository | [https://github.com/nasa/fprime](https://github.com/nasa/fprime) |
| Community Forum and Mailing List | [https://groups.google.com/d/forum/fprime-community](https://groups.google.com/d/forum/fprime-community) |
| Community GitHub Organization | [https://github.com/fprime-community](https://github.com/fprime-community) |
| Standard Reference Application | [Ref](https://github.com/nasa/fprime/blob/master/Ref/README.md)  |
| Raspberry PI Reference Application | [RPI](https://github.com/nasa/fprime/blob/master/RPI/README.md) |
| Architecture Overview | [Architecture](./Architecture/FPrimeArchitectureShort.pdf) |

## F´ System Requirements

In order to develop applications with F´, the following requirements of the user's system must be met.

1. Linux or Mac OS X operating system (or Windows Subsystem for Linux on Windows)
2. CMake [https://cmake.org/download/](https://cmake.org/download/) available on the system path
3. Bash or Bash compatible shell
4. Clang or GCC compiler
5. Python 3 and PIP [https://www.python.org/downloads/](https://www.python.org/downloads/)


## Quick Installation Guide

F´ can be quickly installed and ready to use by cloning the GitHub repository, installing Python code (typically in a
virtual environment), and building on of our reference applications. For full install instructions please see:
[INSTALL.md](./INSTALL.md).

**Clone and Install**
```
git clone https://github.com/nasa/fprime.git
cd fprime
pip install --upgrade wheel setuptools pip
pip install Fw/Python Gds/
```
**Build the Ref Application**
```
cd Ref
fprime-util generate
fprime-util build
```
**Run the Ref Application**
```
./bin/*/Ref
...
CTRL-C
```

## Further References

Full information on the code and F´ are available at our GitHub page:
[http://github.com/nasa/fprime](http://github.com/nasa/fprime).  

To start with, follow the [installation guide](./INSTALL.md). Then inspect
either the [reference application](https://github.com/nasa/fprime/blob/master/Ref/README.md), 
[raspberry pi reference](https://github.com/nasa/fprime/blob/master/RPI/README.md), or the
[tutorials](Tutorials/README.md).
