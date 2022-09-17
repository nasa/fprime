ARG VARIANT="22.04"
FROM ubuntu:${VARIANT} as base

ARG DEBIAN_FRONTEND=noninteractive

# Install required packages as well as setup the prettier bash prompt and a
# Python virtual environment.
RUN apt-get update && apt-get install -y --no-install-recommends \
    openssh-server \
    wget \
    curl \
    gnupg \
    git \
    build-essential \
    cmake \
    gdb-multiarch \
    default-jre \
    python3 \
    python3-setuptools \
    python3-pip \
    python3-venv

RUN wget https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz \
    && mkdir -p /arm-none-eabi \
    && tar -xf arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz -C /arm-none-eabi \
    && rm arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz \
    # symlink
    && ln -s /arm-none-eabi/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc \
    && ln -s /arm-none-eabi/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++ \
    && ln -s /arm-none-eabi/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb \
    && ln -s /arm-none-eabi/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size \
    && ln -s /arm-none-eabi/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy \
    # install and link deps
    && apt install -y --no-install-recommends libncurses-dev \
    && ln -s /usr/lib/x86_64-linux-gnu/libncurses.so.6 /usr/lib/x86_64-linux-gnu/libncurses.so.5 \
    && ln -s /usr/lib/x86_64-linux-gnu/libtinfo.so.6 /usr/lib/x86_64-linux-gnu/libtinfo.so.5

RUN git config --global --add safe.directory /workspaces/fprime \
    && rm -rf /var/lib/apt/lists/* \
    && python3 -m venv /tmp/fprime-venv \
    && . /tmp/fprime-venv/bin/activate \
    && python3 -m pip install -U --upgrade pip setuptools setuptools_scm wheel \
    && printf '\n[ -d "%s" ] && . %s/bin/activate\n' /tmp/fprime-venv /tmp/fprime-venv >> ~/.bashrc \
    && echo 'eval "$(register-python-argcomplete fprime-cli)"' >> ~/.bashrc
