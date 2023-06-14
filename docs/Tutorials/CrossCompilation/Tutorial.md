# F´ Cross-Compilation Tutorial

## Table of Contents

* <a href="#Introduction">1. Introduction</a>
  * <a href="#Prerequisites">1.1. Prerequisites</a>
  * <a href="#Installing Dependencies">1.2. Installing Dependencies</a>
  * <a href="#Installing the Toolchain">1.3. Installing the Toolchain</a>
* <a href="#Configuring for Cross-Compilers">2. Configuring for Cross-Compilers
  * <a href="#Loading CMake Files">2.1. Loading CMake Files</a>
* <a href="#Compiling for ARM">3. Compiling for ARM</a>
* <a href="#Conclusion">4. Conclusion</a>
* <a href="#Troubleshooting">5. Troubleshooting</a>

<a name="Introduction"></a>
## 1. Introduction

In this section, we will learn how to cross-compile for different architectures.
This tutorial will use the Raspberry Pi ARM x64 as an example. In order to fully
benefit from this tutorial, the user should acquire a Raspberry Pi.

The user should also have an understanding of the Raspberry
Pi and specifically how to SSH into the Pi and run applications.

<a name="Prerequisites"></a>
## 1.1. Prerequisites

To run through this tutorial, you must have a computer that meets the following basic requirements.

1. Computer running Windows 10, Mac OS X, or Ubuntu
2. Administrator access
3. 5GB of free disk space, 8 GB of RAM
4. Knowledge of the command line for your operating system (Bash, Powershell, Zsh, etc).

<a name="Installing Dependencies"></a>
## 1.2. Installing Dependencies

Choose the operating system you are using to install F Prime:

[Windows 10/11 WSL](./Windows.md)

[Mac OS X](./Mac.md)

[Ubuntu 20.04 / 22.04 / Generice Linux](./Linux.md)

<a name="Installing the Toolchain"></a>
## 1.3. Installing the Toolchain

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
<a name="Configuring for Cross-Compilers"></a>
## 2. Configuring for Cross-Compilers

In this section we will configure the installed compilers to be used to generate F' binaries 
to be run on Embedded Arm Linux.

<a name="Loading CMake Files"></a>
## 2.1 Loading CMake Files

In the Introduction section (hyperlink) for this tutorial the ARM Linux cross-compilers 
were installed. However, the F' CMake systemw ill need the appropriate integration files 
to build. These files are called "CMake Toolchain". There is an F' package that provides 
cross-compiler integration for standard ARM Linux cross-compilers (i.e. those provided 
as binary downloads from ARM).

To install the F' ARM package, add it as a submodule via `git`:

```sh
# In Project Directory
git submodule add https://github.com/fprime-community/fprime-arm-linux.git
git submodule update --init --recursive
```

Next add the package to your project's deployment `settings.ini` file. Add the 
following to the `[fprime]` section of the file:

```cmake
[fprime]
...
library_locations: ../fprime-arm-linux
```
> Now is a good time to ensure that the ARM toolchains were installed properly. 
>To test run the following command:
> ```shell
> # For  64-bit ARM hardware
> /opt/toolchains/bin/aarch64-none-linux-gnu-gcc -v
> 
> # For 32-bit ARM hardware
> /opt/toolchains/bin/arm-linux-gnueabi-gcc -v
> ```
>
> Any output other than "file/command not found" is good.
> 
> macOS users must run these commands from within the Docker container described 
> in [Appendix I](./appendix-1.md).

<a name="Compiling for ARM"></a>
## 3. Compiling for ARM

Now cross-compiling is as easy as building the deployment for a specific platform. 
For users running on 64-bit arm the platform is called `aarch64-linux`, and for users 
on 32-bit linux use `arm-hf-linux`. This package expects the environment variable 
`ARM_TOOLS_PATH` to point to the installation directory of the ARM cross-compilers.

> Users need to generate for each platform they wish to run on. We previously generated 
> for our host machine.

Here is how to build for the 64-bit Arm Linux platform:

```sh
export ARM_TOOLS_PATH=/opt/toolchains

#You can check to make sure the environment variable is set by running:
echo $ARM_TOOLS_PATH

#This should return the path /opt/toolchains

# For in-person workshops and ARM 64-bit hardware
# In Deployment Folder
fprime-util generate aarch64-linux
fprime-util build aarch64-linux

# For ARM 32-bit hardware
# In Deployment Folder
fprime-util generate arm-hf-linux
fprime-util build arm-hf-linux
```
> macOS users must run from within the Docker container.

## Running on Hardware

Now it is time to run on hardware. For this tutorial, the assumption is that the Arm 
Linux machine is available on the network, is running SSH, and the username, password, 
device address, and host address are known. Without this configuration, users should 
skip to the next section of the tutorial.

First, in a terminal upload the software to hardware platform. This is done with:

```sh
# For ARM 64-bit hardware
# In Deployment Folder
scp -r build-artifacts/aarch64-linux <username>@<device-address>:[NamyOfBinary]

# For ARM 32-bit hardware
# In Deployment Folder
scp -r build-artifacts/arm-hf-linux <username>@<device-address>:[NameOfBinary]
```
> Users must fill in the username and device address above.

Next run the F´ GDS without launching the native compilation (`-n`) and with the 
dictionary from the build above (`--dictionary ./build-artifacts/<platform name>/<.xml document>`).

```sh
# For in-person workshops and ARM 64-bit hardware
# In Deployment Folder
fprime-gds -n --dictionary build-artifacts/aarch64-linux/dict/<App Dictionary>.xml

# For ARM 32-bit hardware
# In Deployment Folder
fprime-gds -n --dictionary build-artifacts/aarch64-linux/dict/<App Dictionary>.xml
```

<a name="Conclusion"></a>
## 4. Conclusion

The Cross-Compilation tutorial has shown us how to cross-compile our our project 
application to the Raspberry Pi. We have seen how to copy our deployment to the 
Raspberry Pi and run the ground system on our host computer to interact with our 
deployment on the Raspberry Pi.

<a name="Troubleshooting"></a>
## 5. Troubleshooting

If you are getting errors for missing Libc.c files, make sure when you generate 
that the logs show that it is using the `/opt/toolchains` path and not `/bin`. 
You can additionally verify that the correct toolchain is being used by watching
the logs scroll by when you initially `fprime-util generate [toolchain]`.