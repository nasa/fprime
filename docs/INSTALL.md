---
layout: default
title: "F´ Installation Guide"
---

## Overview

This installation guide is specifically designed to enable individuals and researchers to get up and running with F´ quickly. Larger projects with specific requirements may need to extend this process.

**Note:** See the troubleshooting section at the bottom for help resolving common issues found during F´ installs.

**Sections:**
- [System Requirements](#system-requirements)
- [Creating a New Project](#creating-a-new-f´-project)
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

> Latest versions of PIP are strongly recommended. See [Recommended PIP Versions](./troubleshooting.md#recommended-pip-versions)

> For build host architectures other than x86_64 or aarch64, and systems with older PIP versions, Java is required

> Ubuntu and Debian users should see notes on [Python installation](./troubleshooting.md#ubuntu-debian-java-and-python-pip)

> macOS users must ensure the [CMake command line utility is on their path](./troubleshooting.md#mac-os-x-and-cmake-command-not-found)

> Other OS-specific notes are in the [Troubleshooting](#troubleshooting) section below.


## Creating a new F´ Project

The ecosystem of tools supporting F´ is installed as Python packages available via PIP. These packages are installed in a [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/) by the F´ Bootstrap tool. It is recommended to have one virtual environment per project.

### 1. Install the F´ Bootstrap tool

The F´ Bootstrap tool is responsible for creating a new F´ project and installing the Python dependencies within the project's virtual environment. Install the fprime-bootstrap tool with:
```
pip install fprime-bootstrap
```

> Some macOS users see an SSL error. [Correct the SSL error](#ssl-error-with-python-38-on-macos) and rerun the above command.

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

1. Ensure a virtual environment for this project has been [created and activated](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/)

2. Download the project
> When using `git` and submodules, remember to run `git submodule update --init --recursive`

4. Install the required F´ tools version
`pip install -r <project>/fprime/requirements.txt`

> Some projects ship their own `requirements.txt`.  Install using that file if it exists.

## Troubleshooting

For users having issues, please see our [troubleshooting guide](./troubleshooting.md).
