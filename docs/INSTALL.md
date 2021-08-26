---
layout: default
title: "F´ Installation Guide"
---

## Overview
This installation guide is specifically designed to enable individuals and researchers to get up and
running with F´ quickly. This guide is not intended for large teams with specific content
management (CM) requirements.

**Note:** see the troubleshooting section at the bottom for help resolving common issues found during F´ installs.

## Requirements

F´ depends on several items before the user should attempt to install it. These requirements are
listed below and should be on any system the user wishes to use. Should these items not be
available on the user's Operating System, then a Virtual Machine should be used. One option is
[VirtualBox](https://www.virtualbox.org/).

Requirements:

1. Linux or Mac OS X operating system (or Windows Subsystem for Linux on Windows)
2. [CMake 3.5](https://cmake.org/download/) or newer. CLI tool must be available on the system path.
3. Bash or Bash compatible shell
4. CLang or GCC compiler
5. [Python 3.6+](https://www.python.org/downloads/) and PIP
6. Python Virtual Environment \* (pip install venv or pip install virtualenv)

You can install them using this one-liner command :

```sudo apt-get update && sudo apt-get -y install cmake git python3-pip``` (for Debian based OS'es)

OR

``` yum install cmake git python3-pip ``` (for RHEL OS)

**Note:** it is possible to install and run F´ without a virtual environment, however; for
individuals and researchers, this is the recommended approach.

### Create and Activate a new Python Environment

This will create a new virtual environment for F´ to be installed into. The following commands
will create a new virtual environment called `fprime-venv` and ensure that the virtual environment
is activated.

**Python 3.6+:**

```
python3 -m venv ./fprime-venv
. ./fprime-venv/bin/activate
```

Any time the user wishes to use F´, this virtual environment should be activated. This should be
done in each new shell the user uses. This is all that is required to activate F´ for use.

**Note:** file system for VirtualEnvironment should support symbolic links, or use the `--always-copy` flag to pip.

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

F´ python-support packages have certain requirements on various Python packages available on PYPI.
These requirements are broken out into two files one for the Autocoder tools and one for the GDS
tools package. This is to enable users to choose which tools they'd like to use.

**Installing F´ Python Packages**

```
pip install --upgrade fprime-tools fprime-gds
```

## Checking Your F´ Installation

The user may easily checkout that their F´ installation has succeeded by testing the following
commands. First, the user will test the build and Autocoder install. Next, the user will test the GDS
layer components. Should the user not have installed the given component, the commands won't work.

**Testing F´ Autocoder Installation Via Building Ref Application**

```
cd Ref
fprime-util generate
fprime-util build --jobs "$(nproc || printf '%s\n' 1)"
```

**Testing F´ GDS Installation Via Running HTML GUI**

```
fprime-gds -g html -r <path to fprime checkout>/Ref/build-artifacts
```

**Note:** `Ref` should contain pre-built dictionaries and binaries for the user's system. This can
be achieved by running the Autocoder installation test (the user must have a working Autocoder
installation).

## (Optional) Installing Tab Completion

Several of F´s command-line utilities support tab completion. To enable these tools to use it, see the [instructions here](UsersGuide/user/autocomplete.md).

## Troubleshooting

This section will add some known hints to trouble-shooting with the installation of F´. This will hopefully help users
install things more effectively.

### Ubuntu, Debian, Python PIP, and Python Virtual Environments

Ubuntu and possibly other Debian variants don't include the virtual environment nor pip packages in the default python 3
installation. To get a fully functional Python 3 installation, use these commands on Ubuntu:

```
sudo apt install python3 python3-pip python3-venv
```

Now you should be able to run the installation without trouble.

### Mac OS X and CMake Command Not Found

If the user chooses to install CMake directly from the CMake site (not using homebrew nor Mac Ports), then the CMake command-line tools must be added to the user's PATH or default system libraries. The quickest command to do that is:

```
sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
```

More information can be found [here](https://stackoverflow.com/questions/30668601/installing-cmake-command-line-tools-on-a-mac)

### System Python, Packages, and Python3

Many operating systems offer python PIP packages through their package manager (apt, yum, etc). Most python projects
recommend avoiding those packages, but rather installing from PIP in a virtual environment. The reason for this is that the
version of the python package from the OS may not be the required version that the python project depends on. Thus, for
F´ we strongly recommend using a virtual environment and not system packages.

If the user chooses to use system python as their python to run F´, they must ensure that "python" available on the path
points to a Python3 install, not the Python 2 installation that most OSes default to. Also, packages F´ uses when installing
may break the system Python, and prevent the OS from functioning. Use at your own risk.
