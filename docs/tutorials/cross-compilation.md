# F´ Cross-Compilation Setup Tutorial

## Table of Contents

1. [Cross-Compilation Setup](#cross-compilation-setup)
1. [Obtaining a Sysroot](#obtaining-a-sysroot)
1. [Cross-Compilation Tutorial - Compiling for ARM](#cross-compilation-tutorial-compiling-for-arm)
1. [F´ Running on ARM Linux Tutorial](#f-running-on-arm-linux-tutorial)

## Cross-Compilation Setup

In this section, we will learn how to install all the dependencies required for cross-compiling for different architectures.
This tutorial will use 64-bit ARM Linux (e.g. a Raspberry Pi 4/5) as an example. In order to fully benefit from this tutorial, the user should acquire such a device.

F´ cross-compiles for 64-bit ARM Linux using the `aarch64-clang-linux` toolchain. This approach uses a generic
clang/lld installation together with a *sysroot* (a directory tree containing the target's C library, C++ library,
and headers). Since clang is inherently a cross-compiler, the same setup works natively on Linux **and** macOS
hosts.

### Setup Prerequisites

To run through this tutorial, you must have a computer that meets the following basic requirements.

1. Computer running Windows 10, Mac OS X, or Ubuntu
2. Administrator access
3. 5GB of free disk space, 8 GB of RAM
4. Knowledge of the command line for your operating system (Bash, Powershell, Zsh, etc).
5. [F Prime Setup](https://fprime.jpl.nasa.gov/latest/docs/getting-started/installing-fprime/)

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

    The clang shipped with Xcode does not include `lld`, so download an official
    [llvm.org release](https://github.com/llvm/llvm-project/releases) for your Mac,
    extract it, and point the `PATH` environment variable at the extracted
    directory (the one containing `bin/`) and also set `LLVM_TOOLS_PATH` to the same path.

    >[!TIP]
    > On newer macOS systems you may need to clear the quarantine flag on the download.

    **Example Commands:** Assuming download of `LLVM-22.1.8-macOS-ARM64.tar.xz`
    ```bash
    xattr -d com.apple.quarantine LLVM-22.1.8-macOS-ARM64.tar.xz
    tar -xf LLVM-22.1.8-macOS-ARM64.tar.xz
    export PATH="`pwd`/LLVM-22.1.8-macOS-ARM64/bin:${PATH}"
    export LLVM_TOOLS_PATH="`pwd`/LLVM-22.1.8-macOS-ARM64/bin:${PATH}"
    ```


=== "Ubuntu 20.04 / 22.04 / Generic Linux"

    **Ubuntu 20.04 / 22.04 / Generic Linux**

    Ensure that your distribution is up to date.

    ```sh
    sudo apt update
    sudo apt install build-essential git cmake python3 python3-venv python3-pip clang lld llvm
    ```

    Alternatively, an official [llvm.org release](https://github.com/llvm/llvm-project/releases)
    may be downloaded and extracted anywhere; point your PATH and LLVM_TOOLS_PATH to the extracted
    folder.

    **Example Commands:** Assuming download of LLVM-22.1.8-Linux-X64.tar.xz
    ```bash
    tar -xf LLVM-22.1.8-Linux-X64.tar.xz
    export PATH="`pwd`/LLVM-22.1.8-Linux-X64/bin:${PATH}"
    export LLVM_TOOLS_PATH="`pwd`/LLVM-22.1.8-Linux-X64/bin:${PATH}"
    ```


To verify the tools are available, run:

```shell
clang --version
ld.lld --version
```

Any output other than "file/command not found" is good.

## Obtaining a Sysroot

The `aarch64-clang-linux` toolchain requires a sysroot for the target: a directory containing the
target's glibc, libstdc++, headers, and GCC runtime files. A ready-made sysroot for 64-bit ARM
Linux targets (e.g. Raspberry Pi 4/5) is published at
[fprime-community/fprime-rpi-5-sysroot](https://github.com/fprime-community/fprime-rpi-5-sysroot/releases).
Download and extract it:

```bash
mkdir -p $HOME/sysroots
chown $USER $HOME/sysroots
curl -Ls https://github.com/fprime-community/fprime-rpi-5-sysroot/archive/refs/tags/v0.1.tar.gz | tar -C $HOME/sysroots -xz
export AARCH64_SYSROOT=$HOME/sysroots/fprime-rpi-5-sysroot-0.1/sysroot-aarch64-none-linux
```

The sysroot contains no host binaries, so the same download works from Linux and macOS hosts.

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
export AARCH64_SYSROOT=$HOME/sysroots/fprime-rpi-5-sysroot-0.1/sysroot-aarch64-none-linux

#You can check to make sure the environment variable is set by running:
echo $AARCH64_SYSROOT

#This should return the path to your sysroot

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
- **Missing headers or libraries**: verify `AARCH64_SYSROOT` points at the extracted sysroot
  directory itself (the one containing `usr/`). See [Obtaining a Sysroot](#obtaining-a-sysroot).
- **`GLIBC_x.yz not found` when running on the target**: the sysroot's glibc is newer than the
  target's. Use a sysroot matching the target OS.
- **Wrong tools picked up**: run `fprime-util generate aarch64-clang-linux -DCMAKE_DEBUG_OUTPUT=ON`
  and watch the logs to verify the expected `clang`/`clang++` and sysroot are used.
