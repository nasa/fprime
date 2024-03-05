---
layout: default
title: "F´ Installation Guide"
---

## Overview

This installation guide is specifically designed to enable individuals and researchers to get up and running with F´ quickly. Larger projects with specific requirements may need to extend this process.

**Note:** See the troubleshooting section at the bottom for help resolving common issues found during F´ installs.

**Sections:**
- [System Requirements](#system-requirements)
- [Setting Up the Development Environment](#setting-up-the-development-environment)
- [Creating a New Project](#creating-a-new-project)
- [Working With An Existing Project](#working-with-an-existing-project)
- [Troubleshooting](#troubleshooting)

  
## System Requirements

F´ depends on several items before the user should attempt to install it. These requirements are listed below and the user should ensure they are installed before proceeding with this guide.

Requirements:

1. Linux, macOS, or WSL on Windows
2. git
3. [CMake 3.16](https://cmake.org/download/) or newer. CLI tool must be available on the system path.
4. CLang or GNU C and C++ compilers (e.g. gcc and g++)
5. [Python 3.8+](https://www.python.org/downloads/), virtual environments, and PIP

> Latest versions of PIP are strongly recommended, as this will install native packages with improved performance and no dependency on Java. See [Recommended PIP Versions](#recommended-pip-versions)

> Ubuntu and Debian users should see notes on [Python installation](#ubuntu-debian-java-and-python-pip)

> macOS users must ensure the [CMake command line utility is on their path](#mac-os-x-and-cmake-command-not-found)

> Other OS-specific notes are in the [Troubleshooting](#Troubleshooting) section below.


## Creating a new F´ Project

The ecosystem of tools supporting F´ is installed as Python packages available via PIP. These packages are installed in a [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/) by the F´ Bootstrap tool. It is recommended to have one virtual environment per project.

### 1. Install the F´ Bootstrap tool

The F´ Bootstrap tool is responsible for creating a new F´ project and installing the Python dependencies within the project's virtual environment. Install the fprime-bootstrap tool with:
```
pip install fprime-bootstrap
```

> Some macOS users see an SSL error. [Correct the SSL error](#ssl-error-with-python-37-on-macos) and rerun the above command.

### 2. Create a new project

The entrypoint to developing with F´ is creating a new project. This will clone the F´ repository and install the full tool suite of the specified version for working with the selected version of F´. To create a new project, run:
```
fprime-bootstrap project
```

This command will ask for some input. Sample responses are below:
```
  [1/1] Project name (MyProject): MyProject
```

This commands perform the following actions:
- Create a new git repository with the standard F´ project structure
- Create a new virtual environment within the project and install dependencies


### 3. Activate the virtual environment

Once the project is created, activate the virtual environment to use the F´ tool suite.

```
cd MyProject
. fprime-venv/bin/activate
```
> Always remember to activate the virtual environment whenever you work with this F´  project.

Next steps: [HelloWorld Tutorial](https://fprime-community.github.io/fprime-tutorial-hello-world/)

## Working With An Existing Project

Sometimes users wish to work with existing F´ projects. Once the project has been acquired, users should install the tools associated with that project. This is done with:

1. Ensure a virtual environment for this project has been created and [activated](#setting-up-the-development-environment)

2. Download the project
> When using `git` and submodules, remember to run `git submodule update --init --recursive`

4. Install the required F´ tools version
`pip install -r <project>/fprime/requirements.txt`

> Some projects ship their own `requirements.txt`.  Install using that file if it exists.

## Troubleshooting

This section will add some known hints to trouble-shooting with the installation of F´. This will hopefully help users install things more effectively.

### fprime-util: command not found

If the user is using a virtual environment and receives the 'command not found', the problem is likely caused by the environment not being sourced in a new terminal. Make sure to source the environment before running:

```
. <path/to/project>/fprime-venv/bin/activate
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


### SSL Error with Python 3.8+ on macOS

> This fix will not work for Python installed via Homebrew.  Try installing Python published at python.org. 

The version of openSSL bundled with Python 3.8+ requires access to macOS's root certificates. If the following error is  encountered while installing fprime: 

```
Failed find expected download: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: unable to get  local issuer certificate
```

Then run the following command in a macOS terminal to install necessary certificates: 

```
cd /Applications/Python\ 3.X/
./Install\ Certificates.command
```

After running above command, re-try installing `fprime-bootstrap`.


### Recommended PIP Versions

Some of the F´ Python packages are built in a way that it is recommended to install them with modern versions of PIP. Systems not recommended or pip versions less than recommended will require Java and run slower versions of FPP tools. The recommended versions are described below:

| System | Recommended `pip` version |
|---|---|
| macOS (10.9+) | >= 21.0.1 |
| x86_64 | >= 19.3 |
| aarch64| >= 20.3 |


