# F´ Cross-Compilation Setup Tutorial

## Table of Contents

1. [Cross-Compilation Setup](#cross-compilation-setup)
1. [Cross-Compilation Tutorial - Compiling for ARM](#cross-compilation-tutorial-compiling-for-arm)
1. [F´ Running on ARM Linux Tutorial](#f-running-on-arm-linux-tutorial)
1. [Appendix I: Installing Rancher Desktop and the F´ ARM Container](#appendix-i-installing-rancher-desktop-and-the-f-arm-container)

## Cross-Compilation Setup

In this section, we will learn how to install all the dependencies required for cross-compiling for different architectures.
This tutorial will use the Raspberry Pi ARM x64 as an example. In order to fully benefit from this tutorial, the user should acquire a Raspberry Pi.

### Setup Prerequisites

To run through this tutorial, you must have a computer that meets the following basic requirements.

1. Computer running Windows 10, Mac OS X, or Ubuntu
2. Administrator access
3. 5GB of free disk space, 8 GB of RAM
4. Knowledge of the command line for your operating system (Bash, Powershell, Zsh, etc).

### Installing Dependencies

Choose the operating system you are using to install F Prime:

=== "Windows 10/11 WSL"

    **Microsoft Windows 10/11 WSL**

    Windows 10 ships with a technology known as WSL. WSL allows users to run Linux virtual machines transparently within the Windows 10 operating system.

    **Powershell: Install WSL with Default Ubuntu**
    ```powershell
    wsl --install
    ```

    To start Ubuntu under WSL, search for Ubuntu in the start menu and select the "Ubuntu on Windows" app. All class commands should be run on these Ubuntu terminals.

    > [!NOTE]
    > Full instructions and troubleshooting help is available in the [Microsoft documentation](https://learn.microsoft.com/en-us/windows/wsl/install).

    **IMPORTANT:**

    Go to the [Ubuntu 20.04 / 22.04 / Generic](#__tabbed_1_3) Linux to finish setting up your WSL environment.


=== "Mac OS X"

    **macOS**

    macOS, like Linux, is a Unix system and thus may be used directly for most of this 
    tutorial. However, Mac users must install the following utilities 
    *and ensure they are available on the command line path*.

    1. [Python 3](https://www.python.org/downloads/release/python-3913/)
    2. [CMake](https://cmake.org/download/)
    3. GCC/CLang typically installed with xcode-select

    **Installing GCC/CLang on macOS**
    ```bash
    xcode-select --install
    ```

    Installing Python and running the above command to install gcc/CLang should ensure 
    that those tools are on the path.

    CMake requires one additional step to ensure it is on the path:

    ```bash
    sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
    ```

    In order to cross-compile, a Linux box is essential. You may choose to use a virtual 
    machine or may choose to follow the instructions in [Appendix I](#appendix-i-installing-rancher-desktop-and-the-f-arm-container) to 
    install a docker container including the necessary tools. 

=== "Ubuntu 20.04 / 22.04 / Generic Linux"

    **Ubuntu 20.04 / 22.04 / Generic Linux**

    Ensure that your distribution is up to date.

    ```sh
    sudo apt update
    sudo apt install build-essential git g++ gdb cmake python3 python3-venv python3-pip
    ```




### Installing the Toolchain

> [!NOTE]
> macOS users must run these commands from within the Docker container described in [Appendix I](#appendix-i-installing-rancher-desktop-and-the-f-arm-container) or setup a Linux virtual machine.

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


## Cross-Compilation Tutorial - Compiling for ARM

In this section, we will learn how to cross-compile for different architectures. This tutorial will use the Raspberry Pi ARM x64 as an example. In order to fully benefit from this tutorial, the user should acquire a Raspberry Pi.

The user should also have an understanding of the Raspberry Pi and specifically how to SSH into the Pi and run applications.


### Compiling for ARM - Prerequisites
Install the dependencies required for compiling for ARM. See the steps in the [Setup Prerequisites](#setup-prerequisites) for more information.


### Compiling for ARM

Cross-compiling is as easy as building the deployment for a specific platform. 
For users running on 64-bit arm the platform is called `aarch64-linux`, and for users 
on 32-bit arm use `arm-hf-linux`. This package expects the environment variable 
`ARM_TOOLS_PATH` to point to the installation directory of the ARM cross-compilers.

> [!NOTE]
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
> [!NOTE]
> macOS users must run these commands from within the Docker container described in [Appendix I](#appendix-i-installing-rancher-desktop-and-the-f-arm-container).

## F´ Running on ARM Linux Tutorial

For this tutorial, the assumption is that the ARM Linux machine is available on the network, is running SSH, and the username, password, device address, and host address are known. Without this configuration, users should skip to the next section of the tutorial.

First, in a terminal upload the software to hardware platform. This is done with:

```sh
# For ARM 64-bit hardware
# In: project root folder
scp -r build-artifacts/aarch64-linux/<name-of-deployment> <username>@<device-address>:deployment

# For ARM 32-bit hardware
# In: project root folder
scp -r build-artifacts/arm-hf-linux/<name-of-deployment> <username>@<device-address>:deployment
```
> Users must fill in the username and device address above.

Next run the F´ GDS without launching the native compilation (`-n`) and with the 
dictionary from the build above (`--dictionary ../build-artifacts/<platform name>/<name-of-deployment>/dict/<.json document>`).

```sh
# For in-person workshops and ARM 64-bit hardware
# In: project root folder
fprime-gds -n --dictionary build-artifacts/aarch64-linux/<name-of-deployment>/dict/<App Dictionary>.json --ip-client --ip-address <device-address>

# For ARM 32-bit hardware
# In: project root folder
fprime-gds -n --dictionary build-artifacts/aarch64-linux/<name-of-deployment>/dict/<App Dictionary>.json --ip-client --ip-address <device-address>
```
> [!NOTE]
> This depends on a flight software deployment that uses TcpServer as the communications driver implementation.

In another terminal SSH into the device and run the uploaded software:
```sh
ssh <username>@<device-address>
deployment/bin/<name-of-deployment> -a 0.0.0.0 -p 50000
```
> User should fill in the username and device address above and ensure the correct executable is supplied.

> If the device does not connect, ensure that the firewall port 50000 is open on the flight computer.

### Troubleshooting

If you are getting errors for missing Libc.c files, make sure when you generate 
that the logs show that it is using the `/opt/toolchains` path and not `/bin`. 
You can additionally verify that the correct toolchain is being used by watching
the logs scroll by when you initially `fprime-util generate <toolchain>`.


## Appendix I: Installing Rancher Desktop and the F´ ARM Container

Some users may with to run cross-compilers within docker to minimize the impact of those tools on their systems. Macintosh users will be required to use docker as the ARM/Linux cross-compilers are not available natively for macOS and simple virtualization of a Linux box is no longer practical since the introduction of M1 and M2 hardware.

### Rancher Desktop Setup

Rancher Desktop is an alternative to Docker Desktop that allows users to run docker containers directly on their desktop  computer. It does not require a license for use like Docker Desktop does and also supports both intel and ARM based  Macintosh computers.

> [!NOTE]
> Non-Macintosh users are advised to run without the below Docker container

To install [Rancher Desktop](https://rancherdesktop.io/), follow the instructions for your operating system. When presented with a "Welcome to Rancher Desktop" dialog, choose the following settings:
1. Disable Kubernetes
2. Select `dockerd`
3. Configure PATH Automatic

![Rancher Config](./img/rancher-config.png)

Ensure that Rancher Desktop is running and that the VM it uses has been started. You can confirm this by ensuring no pop-ups nor progress bars are visible in Rancher Desktop's main window as shown below.

![Rancher Main Window](./img/rancher-running.png)

Once this is done, users can install the container by running the following command in their host terminal. It should  complete without errors.

```bash
docker pull nasafprime/fprime-arm:latest
```

### Running The Container

In order to run the commands provided by the docker container (i.e. the cross-compilers), users must start the container and attach to a terminal inside. This should be done **after** the user has created a project to work within.

To run this container, users may wish to download [this script](https://github.com/fprime-community/fprime-workshop-led-blinker/blob/main/bin/macos-docker) to a `bin` directory in the root of their project. This will start the docker container with appropriate settings. 

Alternatively, the user may run the following command to start the terminal
```bash 
docker run --platform=linux/amd64 --net host -e USER=$USER -u "`id -u`:`id -g`" -v "/path/to/project:/project" -it nasafprime/fprime-arm:latest
```

> [!NOTE]
> Anytime Macintosh users run cross-compilation commands, they **must** do so in a terminal inside the docker container.
