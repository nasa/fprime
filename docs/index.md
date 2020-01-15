---
title: F´: A Flight Software Framework
layout: default
---
# F´ A Flight-Proven Flight Software and Embedded Systems Framework

F´ is a software framework for rapidly development and deployment of embedded systems and spaceflight applications.
Originally developed at NASA's Jet Propulsion Laboratory, F´ is open source software that has been successfully deployed
for several space applications. It has been used for, but is not limited to, CubeSats, SmallSats, instruments, and
deployables.

F´ has the following features:
* Component architecture with well-defined interfaces
* C++ framework providing core capabilities like queues, threads, and operating-system abstraction
* Tools for designing systems and automatically generating code from systems design
* A standard library of flight-worthy components
* Testing tools for unit and system-level testing

F´ is available on GitHub at: [https://github.com/nasa/fprime](https://github.com/nasa/fprime). Further information can
be found at that location.  Interested users may also join our mailing list at:
[https://groups.google.com/d/forum/fprime-community](https://groups.google.com/d/forum/fprime-community) or browse our
community repositories at [https://github.com/fprime-community](https://github.com/fprime-community).

## F´ System Requirements

In order to develop applications with F´ the following requirements of the user's system must be met.

1. Linux or Mac OS X operating system (or Windows Subsystem for Linux on Windows)
2. CMake [https://cmake.org/download/](https://cmake.org/download/) available on the system path
3. Bash or Bash compatible shell
4. CLang or GCC compiler
5. Python 3 and PIP [https://www.python.org/downloads/](https://www.python.org/downloads/)


## Quick Installation Guide

F´ can be quickly installed and ready to use by cloning the GitHub repository, installing Python code (typically in a
virtual environment), and building on of our reference applications. For full install instructions please see:
[INSTALL.md](https://github.com/nasa/fprime/blob/master/INSTALL.md).

**Clone and Install**
```
git clone https://github.com/nasa/fprime.git
cd fprime
pip install Fw/Python Gds/
```
**Build the Ref Application**
```
cd Ref
fprime-util generate
fprime-util install
```
**Run the Ref Application**
```
./bin/*/Ref
...
CTRL-C
```

## Further References

Full information on the code and F´ are available at our Github page:
[http://github.com/nasa/fprime](http://github.com/nasa/fprime).

Tutorials and Getting Started Guides are available in our tutorials section: [Tutorials](Tutorials/README.md).

There are two reference applications, a standard reference [Ref](../Ref/README.md) and one for the Raspberry PI
[RPI](../RPI/README.md).
