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
2. [CMake 3.16](https://cmake.org/download/) or newer. CLI tool must be available on the system path.
3. CLang or GCC compiler
4. [Python 3.7+](https://www.python.org/downloads/) and PIP

**Note:** operating system specific notes are in the [Troubleshooting](#Troubleshooting) section below.

### Cloning the F´ Repository

Installation of F´ requires cloning of the F´ core repository. This uses Git. This will bring
down the core framework and C++ files.

**Clone F´**

```
git clone https://github.com/nasa/fprime
```

### Installing F´ Python Requirements

The ecosystem of tools supporting F´ is installed as python packages available via PIP. The tools
associated with the F´ version just cloned can be installed using the following PIP command. Alternative
tool installations are described in [Advanced](#Advanced).

>Note: Python is used by many operating systems. To prevent problems users are encouraged to run F´ python
>from within a [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/)

**Installing F´ Python Packages**

```
pip install fprime/
```

> Note: the trailing slash is important as that signifies that the locally cloned F´ repository should be
used to determine tools version. Otherwise, the latest F´ tools configuration will be used.

> Note: On some systems pip places user-installed tools in the `.local` folder.
This results in a `fprime-util: command not found` error when trying to run
fprime-util. You will need to add `.local` to your `$PATH` or run as an admin.

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

## Advanced

There are several advanced options users can consider while installing F´. However, users should be warned that some
knowledge of our tools and versions are often required.


### Installing Tab Completion

Several of F´s command-line utilities support tab completion. To enable these tools to use it, see the
[instructions here](UsersGuide/user/autocomplete.md).


### Installing F´ Tools Without Cloning The F´ Repository

Sometimes users wish to install the F´ tools without cloning the F´ repository.  This can be accomplished by identifying
the version of F´ being used and installing that version from PIP.  It will pull in the correct other packages.

**Example**
```
pip install fprime==v3.1.0
```

## Installing Individual Tools

Each of the F´ tools can be installed individually through sub packages. However, users should use care to ensure that
compatible versions are installed.

| Tool(s)     | Package      |
|-------------|--------------|
| fpp         | fprime-fpp   |
| fprime-util | fprime-tools |
| GDS         | fprime-gds   |

> Note: `fprime-gds` depends on `fprime-tools`. It is advisable to install `fprime-gds` and let PIP pull in a compatible
version of `fprime-tools`.

### Installing Optional Components

Some F´ packages contain optional components. These components can be installed including the option in square brackets.

| Package                  | Description |
|--------------------------|-------------|
| fprime-tools[dev]        | Installs the Python development tools for working with F´ Python code |
| fprime-gds[uart-adapter] | Installs the UART communication adapter for the F´ GDS |
| fprime-gds[test-api-xls] | Installs excel logging for use within the integration test framework |


> Note: these packages are optional as they pull in many additional packages and libraries. These may not be supported
on all systems.

**Example:**
```
pip install fprime-gds[uart-adapter]
```


## Troubleshooting

This section will add some known hints to trouble-shooting with the installation of F´. This will hopefully help users
install things more effectively.

### Ubuntu, Debian, Java and Python PIP

Ubuntu and possibly other Debian variants don't include the pip packages in the default python 3 installation. To get a
fully functional, use these commands on Ubuntu and Debian based systems:

```
sudo apt install git cmake default-jre python3 python3-pip
```

Now you should be able to run the installation without trouble.

### Mac OS X and CMake Command Not Found

If the user chooses to install CMake directly from the CMake site (not using homebrew nor Mac Ports), then the CMake
command-line tools must be added to the user's PATH or default system libraries. The quickest command to do that is:

```
sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
```

More information can be found [here](https://stackoverflow.com/questions/30668601/installing-cmake-command-line-tools-on-a-mac)

### System Python, Packages, and Python3

Many operating systems offer python PIP packages through their package manager (apt, yum, etc). Most python projects
recommend avoiding those packages, but rather installing from PIP in a virtual environment. The reason for this is that
the version of the python package from the OS may not be the required version that the python project depends on. Thus,
users may choose to install F´ into a virtual environment. This is outside the scope of this document.
