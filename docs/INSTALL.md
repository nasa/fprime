---
layout: default
title: "F´ Installation Guide"
---

## Overview

This installation guide is specifically designed to enable individuals and researchers to get up and
running with F´ quickly. Larger projects with specific requirements may need to extend this process.

**Note:** See the troubleshooting section at the bottom for help resolving common issues found during F´ installs.

## Requirements

F´ depends on several items before the user should attempt to install it. These requirements are
listed below and the user should ensure they are installed before proceeding with this guide. Should these items not
be available on the user's Operating System, then a Virtual Machine should be used. One option is
[VirtualBox](https://www.virtualbox.org/).

Requirements:

1. Linux or macOS operating system
2. git
3. [CMake 3.16](https://cmake.org/download/) or newer. CLI tool must be available on the system path.
4. CLang or GNU C and C++ compilers (e.g. gcc and g++)
5. [Python 3.7+](https://www.python.org/downloads/), virtual environments, and PIP

**Note:** OS-specific notes are in the [Troubleshooting](#Troubleshooting) section below.

### Setting Up F´ Environment

The ecosystem of tools supporting F´ is installed as python packages available via PIP. In order to install F´, the user should
setup an environment to run these tools in.

>Note: Python is used by many operating systems. To prevent problems users are encouraged to run F´ python
>from within a [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/)

**Setting Up a Virtual Environment**

Choose a location to generate a virtual environment. This can be any path the user has read and write access to. In this guide we'll
use the path: `$HOME/fprime-venv`

```
python3 -m venv $HOME/fprime-venv
. $HOME/fprime-venv/bin/activate
pip install -U setuptools setuptools_scm wheel pip
```

> Note: `. $HOME/fprime-venv/bin/activate` must be run in each new terminal where the user wishes to use the virtual environment.

### Cloning the F´ Repository and Installing F´ Tools

Installation of F´ requires cloning of the F´ core repository via Git. This will bring down the core framework and C++ files.
F´ ships with a `requirements.txt` file enumerating the tools F´ uses and their specific tested versions. 

**Clone F´ and Install Tools**

```
git clone https://github.com/nasa/fprime
pip install -r fprime/requirements.txt
```

>Note: When changing F´ versions make sure to re-run the `pip install -r fprime/requirements.txt` to get the tested tool versions.
>Note: Alternative tool installations are described in [Advanced](#Advanced).

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
knowledge of our tools and versions is often required.


### Installing Tab Completion

Several of F´s command-line utilities support tab completion. To enable these tools to use it, see the
[instructions here](UsersGuide/user/autocomplete.md).


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

### fprime-util: command not found

If the user is using a virtual environment and receives the 'command not found', the problem is likely caused by the
environment not being sourced in a new terminal. Make sure to source the environment before running:

```
. $HOME/fprime-venv/bin/activate
```

If installing without a virtual environment, PIP occasionally uses `$HOME/.local/bin` as a place to install user tools.
Users running without virtual environments should add this directory to the path.


### Ubuntu, Debian, Java and Python PIP

Ubuntu and possibly other Debian variants don't include the pip packages in the default Python 3 installation. To get fully functional, use these commands on Ubuntu and Debian based systems:

```
sudo apt install git cmake default-jre python3 python3-pip python3-venv
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
recommend avoiding those packages and instead installing them from PIP in a virtual environment. The reason for this is that the version of the python package from the OS may not be the required version that the python project depends on. Thus, users may choose to install F´ into a virtual environment. This is outside the scope of this document.

### SSL Error with Python 3.6+ on macOS

The version of openSSL bundled with Python 3.6+ requires access to macOS's root certificates. If the following error is 
encountered while installing fprime: 

```
Failed find expected download: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: unable to get 
local issuer certificate
```

Then run the following command in a macOS terminal to install necessary certificates: 

```
cd /Applications/Python\ 3.X/
./Install\ Certificates.command
```

After running above command, re-try installing fprime.  
