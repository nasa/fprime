---
layout: default
title: "F´ Installation Guide"
---

## Overview

This installation guide is specifically designed to enable individuals and researchers to get up and running with F´ quickly. Larger projects with specific requirements may need to extend this process.

**Note:** See the troubleshooting section at the bottom for help resolving common issues found during F´ installs.

## Requirements

F´ depends on several items before the user should attempt to install it. These requirements are listed below and the user should ensure they are installed before proceeding with this guide.

Requirements:

1. Linux or macOS operating system
2. git
3. [CMake 3.16](https://cmake.org/download/) or newer. CLI tool must be available on the system path.
4. CLang or GNU C and C++ compilers (e.g. gcc and g++)
5. [Python 3.7+](https://www.python.org/downloads/), virtual environments, and PIP

**Note:** OS-specific notes are in the [Troubleshooting](#Troubleshooting) section below.

### Bootstrapping the F´ Development Environment

The ecosystem of tools supporting F´ is installed as python packages available via PIP.

```
pip install fprime-tools
```

> Python is used by many operating systems. To prevent problems users are encouraged to run F´ python from within a [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/). This is set up when creating a new F´ project.


### Creating a New F´ Project

The entrypoint to developing with F´ is creating a new project. This will clone the F´ repository and install the necessary tool versions for working with the specified version of F´.
```
fprime-util new --project
```
This command will ask for some input. Sample responses are below:
```
project_name [MyProject]: MyProject
fprime_branch_or_tag [devel]: devel
Select install_venv:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
```

Next steps: [HelloWorld Tutorial](Tutorials/HelloWorld/Tutorial.md)

## Advanced

There are several advanced options users can consider while installing F´. However, users should be warned that some knowledge of our tools and versions is often required.


### Installing Tab Completion

Several of F´s command-line utilities support tab completion. To enable these tools to use it, see the [instructions here](UsersGuide/user/autocomplete.md).


### Installing Optional Components

Some F´ packages contain optional components. These components can be installed including the option in square brackets.

| Package                  | Description |
|--------------------------|-------------|
| fprime-tools[dev]        | Installs the Python development tools for working with F´ Python code |
| fprime-gds[uart-adapter] | Installs the UART communication adapter for the F´ GDS |
| fprime-gds[test-api-xls] | Installs excel logging for use within the integration test framework |


> These packages are optional as they pull in many additional packages and libraries. These may not be supported on all systems.

**Example:**
```
pip install fprime-gds[uart-adapter]
```


## Troubleshooting

This section will add some known hints to trouble-shooting with the installation of F´. This will hopefully help users install things more effectively.

### fprime-util: command not found

If the user is using a virtual environment and receives the 'command not found', the problem is likely caused by the environment not being sourced in a new terminal. Make sure to source the environment before running:

```
. $HOME/fprime-venv/bin/activate
```

If installing without a virtual environment, PIP occasionally uses `$HOME/.local/bin` as a place to install user tools. Users running without virtual environments should add this directory to the path.

### Helper script 'fpp-redirect-helper' exited with reason: Permission denied

This error can occur when the helper-script, *(`fprime/cmake/autocoder/fpp-wrapper/fpp-redirect-helper`)* loses its execution permission.

To verify that this is the case, change to the directory containing `fpp-redirect-helper` and verify that it is executable.

* `cd fprime/cmake/autocoder/fpp-wrapper/`
* `ls -l`

If it is not executable, add the permission back.

* `chmod 700 fpp-redirect-helper`

### Ubuntu, Debian, Java and Python PIP

Ubuntu and possibly other Debian variants don't include the pip packages in the default Python 3 installation. To get fully functional, use these commands on Ubuntu and Debian based systems:

```
sudo apt install git cmake default-jre python3 python3-pip python3-venv
```

Now you should be able to run the installation without trouble.

### Mac OS X and CMake Command Not Found

If the user chooses to install CMake directly from the CMake site (not using homebrew nor Mac Ports), then the CMake command-line tools must be added to the user's PATH or default system libraries. The quickest command to do that is:

```
sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
```

More information can be found [here](https://stackoverflow.com/questions/30668601/installing-cmake-command-line-tools-on-a-mac)

### System Python, Packages, and Python3

Many operating systems offer python PIP packages through their package manager (apt, yum, etc). Most python projects recommend avoiding those packages and instead installing them from PIP in a virtual environment. The reason for this is that the version of the python package from the OS may not be the required version that the python project depends on. Thus, users may choose to install F´ into a virtual environment. This is outside the scope of this document.

### SSL Error with Python 3.7+ on macOS

The version of openSSL bundled with Python 3.7+ requires access to macOS's root certificates. If the following error is  encountered while installing fprime: 

```
Failed find expected download: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: unable to get  local issuer certificate
```

Then run the following command in a macOS terminal to install necessary certificates: 

```
cd /Applications/Python\ 3.X/
./Install\ Certificates.command
```

After running above command, re-try installing fprime.  
