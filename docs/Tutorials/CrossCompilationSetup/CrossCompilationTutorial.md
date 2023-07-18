# F´ Cross-Compilation Tutorial

## Table of Contents

* <a href="#1-introduction">1. Introduction</a>
* <a href="#2-compiling-for-arm">2. Compiling for ARM</a>
* <a href="#3-troubleshooting">3. Troubleshooting</a>


<a name="Introduction"></a>
## 1. Introduction

In this section, we will learn how to cross-compile for different architectures. This tutorial will use the Raspberry Pi ARM x64 as an example. In order to fully benefit from this tutorial, the user should acquire a Raspberry Pi.

The user should also have an understanding of the Raspberry Pi and specifically how to SSH into the Pi and run applications.


### Prerequisites
Install the dependencies required for compiling for ARM. See the steps in the [Cross-Compilation Setup Tutorial](./CrossCompilationSetupTutorial.md) for more information.


<a name="Compiling for ARM"></a>
## 2. Compiling for ARM

Cross-compiling is as easy as building the deployment for a specific platform. 
For users running on 64-bit arm the platform is called `aarch64-linux`, and for users 
on 32-bit arm use `arm-hf-linux`. This package expects the environment variable 
`ARM_TOOLS_PATH` to point to the installation directory of the ARM cross-compilers.

> Users need to generate for each platform they wish to run on.

Here is how to build for the 64-bit Arm Linux platform:

```sh
export ARM_TOOLS_PATH=/opt/toolchains

#You can check to make sure the environment variable is set by running:
echo $ARM_TOOLS_PATH

#This should return the path /opt/toolchains

# For in-person workshops and ARM 64-bit hardware
# In: Deployment Folder
fprime-util generate aarch64-linux
fprime-util build aarch64-linux

# For ARM 32-bit hardware
# In: Deployment Folder
fprime-util generate arm-hf-linux
fprime-util build arm-hf-linux
```
> Note: macOS users must run these commands from within the Docker container described in [Appendix I](./appendix-1.md).