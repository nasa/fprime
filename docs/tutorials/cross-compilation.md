# F´ Cross-Compilation Setup Tutorial

## Table of Contents

1. [Cross-Compilation Setup](#cross-compilation-setup)
1. [Obtaining a Sysroot](#obtaining-a-sysroot)
1. [Cross-Compilation Tutorial - Compiling for ARM](#cross-compilation-tutorial-compiling-for-arm)
1. [F´ Running on ARM Linux Tutorial](#f-running-on-arm-linux-tutorial)
1. [Appendix I: Legacy GNU Cross-Toolchains](#appendix-i-legacy-gnu-cross-toolchains)

## Cross-Compilation Setup

In this section, we will learn how to install all the dependencies required for cross-compiling for different architectures.
This tutorial will use 64-bit ARM Linux (e.g. a Raspberry Pi 4/5) as an example. In order to fully benefit from this tutorial, the user should acquire such a device.

F´ cross-compiles for 64-bit ARM Linux using the `aarch64-clang-linux` toolchain. This approach uses a generic
clang/lld installation together with a *sysroot* (a directory tree containing the target's C library, C++ library,
and headers). Because clang is inherently a cross-compiler, the same setup works natively on Linux **and** macOS
hosts — no per-host GNU cross-toolchain (nor Docker container on macOS) is required.

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
    tutorial. Mac users must install the following utilities
    *and ensure they are available on the command line path*.

    1. [Python 3](https://www.python.org/downloads/release/python-3913/)
    2. [CMake](https://cmake.org/download/)
    3. clang and lld

    The clang shipped with Xcode does not include `lld`, so install a complete LLVM
    distribution. Either install via Homebrew:

    ```bash
    brew install llvm
    export LLVM_TOOLS_PATH=$(brew --prefix llvm)
    ```

    or download an official [llvm.org release](https://github.com/llvm/llvm-project/releases)
    for your Mac, extract it, and point `LLVM_TOOLS_PATH` at the extracted directory.

    CMake requires one additional step to ensure it is on the path:

    ```bash
    sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
    ```

=== "Ubuntu 20.04 / 22.04 / Generic Linux"

    **Ubuntu 20.04 / 22.04 / Generic Linux**

    Ensure that your distribution is up to date.

    ```sh
    sudo apt update
    sudo apt install build-essential git cmake python3 python3-venv python3-pip clang lld llvm
    ```

    Alternatively, an official [llvm.org release](https://github.com/llvm/llvm-project/releases)
    may be downloaded and extracted anywhere; point the `LLVM_TOOLS_PATH` environment variable
    at the extracted directory.

To verify the tools are available, run:

```shell
clang --version
ld.lld --version
```

Any output other than "file/command not found" is good. If the tools are not on your `PATH`,
set `LLVM_TOOLS_PATH` to the root of your LLVM installation (the directory containing `bin/`).

## Obtaining a Sysroot

The `aarch64-clang-linux` toolchain requires a sysroot for the target: a directory containing the
target's glibc, libstdc++, headers, and GCC runtime files. There are several ways to obtain one:

1. **Download a pre-built sysroot.** Projects typically publish a sysroot tarball (e.g. as a GitHub
   Release asset) matched to their target OS. If your project provides one, download and extract it:

    ```bash
    sudo mkdir -p /opt/sysroots
    sudo chown $USER /opt/sysroots
    curl -Ls <sysroot-tarball-url> | tar -C /opt/sysroots -xz
    ```

2. **Extract one from a GNU cross-toolchain.** The Arm GNU toolchain releases contain a complete
   sysroot (glibc 2.31 based, compatible with recent Raspberry Pi OS and Ubuntu targets). The
   following commands download the toolchain and assemble a standalone sysroot from its `libc`
   tree, libstdc++ headers, and GCC runtime:

    ```bash
    sudo mkdir -p /opt/sysroots /tmp/arm-gnu
    sudo chown $USER /opt/sysroots
    curl -Ls https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu.tar.xz | tar -JC /tmp/arm-gnu --strip-components=1 -x

    export SYSROOT=/opt/sysroots/aarch64-none-linux-gnu
    mkdir -p $SYSROOT
    cp -a /tmp/arm-gnu/aarch64-none-linux-gnu/libc/. $SYSROOT/
    mkdir -p $SYSROOT/usr/include/c++
    cp -a /tmp/arm-gnu/aarch64-none-linux-gnu/include/c++/10.2.1 $SYSROOT/usr/include/c++/
    mkdir -p $SYSROOT/usr/lib/gcc/aarch64-none-linux-gnu
    cp -a /tmp/arm-gnu/lib/gcc/aarch64-none-linux-gnu/10.2.1 $SYSROOT/usr/lib/gcc/aarch64-none-linux-gnu/
    cp -a /tmp/arm-gnu/aarch64-none-linux-gnu/lib64/. $SYSROOT/usr/lib64/
    rm -rf /tmp/arm-gnu
    ```

3. **Copy one from the target device.** For an exact match with the deployed OS, copy the relevant
   directories (`/lib`, `/usr/lib`, `/usr/include`) from the running target into a local sysroot
   directory, e.g. using `rsync`.

> [!NOTE]
> The sysroot's glibc version must be no newer than the glibc on the target device, or the
> resulting binaries will fail to run with "GLIBC_x.yz not found" errors.

## Cross-Compilation Tutorial - Compiling for ARM

In this section, we will learn how to cross-compile for different architectures. This tutorial will use the Raspberry Pi ARM x64 as an example. In order to fully benefit from this tutorial, the user should acquire a Raspberry Pi.

The user should also have an understanding of the Raspberry Pi and specifically how to SSH into the Pi and run applications.


### Compiling for ARM - Prerequisites
Install the dependencies and sysroot as described above. See the steps in the [Setup Prerequisites](#setup-prerequisites) and [Obtaining a Sysroot](#obtaining-a-sysroot) sections for more information.


### Compiling for ARM

Cross-compiling is as easy as building the deployment for a specific platform. For users running
on 64-bit ARM Linux the platform is called `aarch64-clang-linux`. This toolchain expects:

- `clang`, `clang++`, and `ld.lld` on the `PATH`, or the environment variable `LLVM_TOOLS_PATH`
  (or `-DLLVM_TOOLS_PATH=...`) pointing at the root of an LLVM installation.
- The environment variable `AARCH64_SYSROOT` (or `-DCMAKE_SYSROOT=...`) pointing at the target
  sysroot.

> [!NOTE]
> Users need to generate for each platform they wish to run on.

Here is how to build for the 64-bit ARM Linux platform:

```sh
export AARCH64_SYSROOT=/opt/sysroots/aarch64-none-linux-gnu

#You can check to make sure the environment variable is set by running:
echo $AARCH64_SYSROOT

#This should return the path /opt/sysroots/aarch64-none-linux-gnu

# In: Deployment Folder
fprime-util generate aarch64-clang-linux
fprime-util build aarch64-clang-linux
```

## F´ Running on ARM Linux Tutorial

For this tutorial, the assumption is that the ARM Linux machine is available on the network, is running SSH, and the username, password, device address, and host address are known. Without this configuration, users should skip to the next section of the tutorial.

First, in a terminal upload the software to hardware platform. This is done with:

```sh
# In: project root folder
scp build-artifacts/aarch64-clang-linux/<name-of-deployment>/bin/<name-of-deployment> <username>@<device-address>:deployment
```
> Users must fill in the username and device address above.

Next run the F´ GDS without launching the native compilation (`-n`) and with the 
dictionary from the build above (`--dictionary ../build-artifacts/<platform name>/<name-of-deployment>/dict/<.json document>`).

```sh
# In: project root folder
fprime-gds -n --dictionary build-artifacts/aarch64-clang-linux/<name-of-deployment>/dict/<App Dictionary>.json --ip-client --ip-address <device-address>
```
> [!NOTE]
> This depends on a flight software deployment that uses TcpServer as the communications driver implementation.

In another terminal SSH into the device and run the uploaded software:
```sh
ssh <username>@<device-address>
./deployment -a 0.0.0.0 -p 50000
```
> User should fill in the username and device address above and ensure the correct executable is supplied.

> If the device does not connect, ensure that the firewall port 50000 is open on the flight computer.

### Troubleshooting

- **Sysroot errors at generate time**: the toolchain emits a fatal error if no sysroot is set.
  Ensure `AARCH64_SYSROOT` is exported (or pass `-DCMAKE_SYSROOT=...`) before running
  `fprime-util generate aarch64-clang-linux`.
- **Missing headers or libraries**: verify the sysroot contains `usr/include`, the libstdc++
  headers (`usr/include/c++/<version>`), and the GCC runtime (`usr/lib/gcc/<triple>/<version>`).
  See [Obtaining a Sysroot](#obtaining-a-sysroot).
- **`GLIBC_x.yz not found` when running on the target**: the sysroot's glibc is newer than the
  target's. Use a sysroot built from (or matching) the target OS.
- **Wrong tools picked up**: run `fprime-util generate aarch64-clang-linux -DCMAKE_DEBUG_OUTPUT=ON`
  and watch the logs to verify the expected `clang`/`clang++` and sysroot are used.


## Appendix I: Legacy GNU Cross-Toolchains

Prior to the clang + sysroot approach, cross-compilation used per-host GNU cross-toolchains via
the `aarch64-linux` (64-bit) and `arm-hf-linux` (32-bit) platforms. These toolchains remain
available and are still the supported route for 32-bit ARM targets.

Install the pre-built packages provided by ARM into the `/opt/toolchains` directory:

```bash
sudo mkdir -p /opt/toolchains
sudo chown $USER /opt/toolchains
# For users running on 64-bit ARM
curl -Ls https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu.tar.xz | tar -JC /opt/toolchains --strip-components=1 -x
# For users running on 32-bit ARM
curl -Ls https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz | tar -JC /opt/toolchains --strip-components=1 -x
```

These packages expect the environment variable `ARM_TOOLS_PATH` to point to the installation
directory of the ARM cross-compilers:

```sh
export ARM_TOOLS_PATH=/opt/toolchains

# For ARM 64-bit hardware
fprime-util generate aarch64-linux
fprime-util build aarch64-linux

# For ARM 32-bit hardware
fprime-util generate arm-hf-linux
fprime-util build arm-hf-linux
```

Note that the GNU cross-toolchains are Linux-only: macOS users following this route must run the
builds inside a Linux virtual machine or the `nasafprime/fprime-arm:latest` Docker container. The
clang + sysroot approach above removes this requirement.
