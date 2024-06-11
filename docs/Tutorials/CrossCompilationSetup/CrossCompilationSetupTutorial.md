# F´ Cross-Compilation Setup Tutorial

## Table of Contents

* <a href="#1-introduction">1. Introduction</a>
  * <a href="#prerequisites"> Prerequisites</a>
* <a href="#2-installing-dependencies">2. Installing Dependencies</a>
* <a href="#3-installing-the-toolchain">3. Installing the Toolchain</a>

<a name="Introduction"></a>
## 1. Introduction

In this section, we will learn how to install all the dependencies required for cross-compiling for different architectures.
This tutorial will use the Raspberry Pi ARM x64 as an example. In order to fully benefit from this tutorial, the user should acquire a Raspberry Pi.

<a name="Prerequisites"></a>
### Prerequisites

To run through this tutorial, you must have a computer that meets the following basic requirements.

1. Computer running Windows 10, Mac OS X, or Ubuntu
2. Administrator access
3. 5GB of free disk space, 8 GB of RAM
4. Knowledge of the command line for your operating system (Bash, Powershell, Zsh, etc).

<a name="Installing Dependencies"></a>
## 2. Installing Dependencies

Choose the operating system you are using to install F Prime:

- [Windows 10/11 WSL](./Windows.md)
- [Mac OS X](./macOS.md)
- [Ubuntu 20.04 / 22.04 / Generic Linux](./Linux.md)

<a name="Installing the Toolchain"></a>
## 3. Installing the Toolchain

> Note: macOS users must run these commands from within the Docker container described in [Appendix I](./appendix-1.md) or setup a Linux virtual machine.

Installing the cross-compiler will use the pre-built packages provided by ARM. Follow these 
instructions to install these tools for the target hardware into the `/opt/toolchains` directory.

```bash
sudo mkdir -p /opt/toolchains
sudo chown $USER /opt/toolchains
# For in-person workshops, and users running on 64-bit ARM
curl -Ls https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu.tar.xz | tar -JC /opt/toolchains --strip-components=1 -x
# For users running on 32-bit ARM
curl -Ls https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz | tar -JC /opt/toolchains --strip-components=1 -x
```

Next, ensure that the ARM toolchains were installed properly. To test, run the following command: 
```shell
# For  64-bit ARM hardware
/opt/toolchains/bin/aarch64-none-linux-gnu-gcc -v 
# For 32-bit ARM hardware
/opt/toolchains/bin/arm-none-linux-gnueabihf-gcc -v
```
 Any output other than "file/command not found" is good.

**Next:** [Compiling for ARM](./CrossCompilationTutorial.md)
