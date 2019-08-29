# F´ Quick Installation Guide

This installation guide is specifically designed to enable individual and researchers to get up and
running with F´ quickly. This guide is not intended for large teams with specific content
management (CM) requirements.

## Requirements

F´ depends on several items before the user should attempt to install it. These requirements are
listed below, and should be on any system the user wishes to use. Should these items not be
available on the user's Operating System, then a Virtual Machine should be used. One option is
VirtualBox [https://www.virtualbox.org/](https://www.virtualbox.org/).

Requirements:

1. Mac OSX or Linux Operating System
2. CMake [https://cmake.org/download/](https://cmake.org/download/)
3. Bash or Bash compatible shell
4. CLang or GCC compiler
5. Python 3 and PIP [https://www.python.org/downloads/](https://www.python.org/downloads/) 
6. Python Virtual Environment \* (pip install venv or pip install virtualenv)

**Note:** it is possible to install and run F´ without a virtual environment, however; for
individuals and researchers, this is the recommended approach.


### Create and Activate a new Python Environment

This will create a new virtual environment for F´ to be installed into. The following commands
will create a new virtual environment called `fprime-venv` and ensure that virtual environment
is activated.

**Python 3.3+:**
```
python3 -m venv ./fprime-venv
. ./fprime-venv/bin/activate
```

**Python 2.7 - 3.2:**
```
virtualenv ./fprime-venv
. ./fprime-venv/bin/activate
```

Any time the user wishes to use F´, this virtual environment should be activated. This should be
done in each new shell the user uses. This is all that is required to activate F´ for use.

**Activate the Virtual Environment**
```
. ./fprime-venv/bin/activate
```

### Cloning the F´ Repository

Installation of F´ requires cloning of the F´ core repository. This uses Git. This will bring
down the core framework and C++ files. 

**Clone F´**
```
git clone https://github.com/nasa/fprime
```


### Installing F´ Python Requirements

F´ python support packages have certain requirements on various Python packages available on PYPI.
These requirements are broken out in two files one for the Autocoder tools and one for the GDS
tools package. This is to enable users to choose which tools they'd like to use.

**Installing F´ Python Packages**
```
cd <path/to/fprime/checkout>
pip install -r ./Autocoders/Python/requirements.txt
pip install -r ./Gds/requirements.txt
```

## Checking Your F´ Installation

The user may easily checkout that their F´ installation has succeeded by testing the following
commands. First the user will test the build and Autocoder install. Next the user will test the GDS
layer components. Should the user not have installed the given component, the commands won't work.

**Testing F´ Autocoder Installation Via Building Ref Application**
```
mkdir build-test
cd build-test
cmake <path/to/fprime/checkout>/Ref
make -j32
make install -j32
```

**Testing F´ GDS Installation Via Running HTML GUI**
```
<path/to/fprime>/Gds/bin/run_deployment.sh -g html -d <path/to/fprime/checkout>/Ref
```
**Note:** `Ref` should contain pre-built dictionaries and binaries for the user's system. This can
be achieved by running the autocoder installation test (the user must have a working autocoder
installation).


