# F´ Reference Projects

[Supported Platforms](./supported-platforms.md) lists reference projects tied to a specific
hardware and OS pair. F´ also has reference projects that are not tied to a particular
platform, and continuous integration (CI) that exercises repositories under
[fprime-community](https://github.com/fprime-community) on an ongoing basis. Both are
listed here.

## Other Reference Projects

Reference repositories under `fprime-community` that are not already covered by a row in the
[Supported Platforms](./supported-platforms.md#supported-platforms) or
[Targeted Platforms](./supported-platforms.md#targeted-platforms-planned-support) tables.

| Project | Description |
| ------- | ----------- |
| [`fprime-amsat-main-board-reference`](https://github.com/fprime-community/fprime-amsat-main-board-reference) | Port of F´ to the PICO Main Sensor Board for the AMSAT CubeSat Simulator |
| [`fprime-amsat-reference`](https://github.com/fprime-community/fprime-amsat-reference) | Port of F´ to the C&DH Pi Zero 2 Board for the AMSAT CubeSat Simulator |
| [`fprime-generic-hub-reference`](https://github.com/fprime-community/fprime-generic-hub-reference) | Reference for applications running with the Hub Pattern |
| [`fprime-phased-deployment-reference`](https://github.com/fprime-community/fprime-phased-deployment-reference) | Reference deployment demonstrating how to run F´ entirely through phases |
| [`fprime-pycubed-baremetal-reference`](https://github.com/fprime-community/fprime-pycubed-baremetal-reference) | PyCubed baremetal reference deployment |
| [`fprime-pycubed-zephyr-reference`](https://github.com/fprime-community/fprime-pycubed-zephyr-reference) | PyCubed Zephyr reference deployment |
| [`fprime-sensors-reference`](https://github.com/fprime-community/fprime-sensors-reference) | Reference project for the `fprime-sensors` library |
| [`fprime-stm32h7-zephyr-reference`](https://github.com/fprime-community/fprime-stm32h7-zephyr-reference) | STM32H7 Zephyr reference deployment |
| [`fprime-yamcs-reference`](https://github.com/fprime-community/fprime-yamcs-reference) | Reference F´ project integrated with YAMCS mission control software |
| [`fprime_cfs_reference`](https://github.com/fprime-community/fprime_cfs_reference) | cFS reference project using applications built with F´ |

## External Repository CI

`ext-*.yml` workflows in this repository's [CI](https://github.com/nasa/fprime/tree/devel/.github/workflows)
keep the reference and tutorial repositories above building against `devel`. The trigger
column matters here: several of these run on a schedule, on release, or only on manual
dispatch, rather than on every push — without that context their badges can read as stale
or broken when they are working as intended.

| Workflow | Repository | Trigger | Status |
| -------- | ---------- | ------- | ------ |
| External Repo: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | push, pull request | [![External Repo: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker.yml) |
| Soak Setup: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | on release, manual dispatch | [![Soak Setup: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-setup.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-setup.yml) |
| Soak Test: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | scheduled | [![Soak Test: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-test.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-test.yml) |
| Soak Summary: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | manual dispatch | [![Soak Summary: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-summary.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-summary.yml) |
| External Repo: fprime-examples | [`fprime-examples`](https://github.com/nasa/fprime-examples) | push, pull request | [![External Repo: fprime-examples](https://github.com/nasa/fprime/actions/workflows/ext-build-examples-repo.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-examples-repo.yml) |
| External Repo: fprime-python-reference | [`fprime-python-reference`](https://github.com/fprime-community/fprime-python-reference) | push, pull request | [![External Repo: fprime-python-reference](https://github.com/nasa/fprime/actions/workflows/ext-build-fprime-python-reference.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-fprime-python-reference.yml) |
| External Repo: Tutorial: HelloWorld | [`fprime-tutorial-hello-world`](https://github.com/fprime-community/fprime-tutorial-hello-world) | push, pull request | [![External Repo: Tutorial: HelloWorld](https://github.com/nasa/fprime/actions/workflows/ext-build-hello-world.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-hello-world.yml) |
| External Repo: Tutorial: LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | push, pull request | [![External Repo: Tutorial: LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-build-led-blinker.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-led-blinker.yml) |
| External Repo: Tutorial: MathComponent | [`fprime-tutorial-math-component`](https://github.com/fprime-community/fprime-tutorial-math-component) | push, pull request | [![External Repo: Tutorial: MathComponent](https://github.com/nasa/fprime/actions/workflows/ext-build-math-comp.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-math-comp.yml) |
| External Repo: Cookiecutters Tests | [`fprime-tools`](https://github.com/nasa/fprime-tools), [`fprime-bootstrap`](https://github.com/fprime-community/fprime-bootstrap) | push, pull request | [![External Repo: Cookiecutters Tests](https://github.com/nasa/fprime/actions/workflows/ext-cookiecutters-test.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-cookiecutters-test.yml) |
| External Repo: cFS Reference | [`fprime_cfs_reference`](https://github.com/fprime-community/fprime_cfs_reference) | push, pull request | [![External Repo: cFS Reference](https://github.com/nasa/fprime/actions/workflows/ext-fprime-cfs-reference.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-cfs-reference.yml) |
| External Repo: Zephyr Reference (Pico 2) | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | push, pull request | [![External Repo: Zephyr Reference (Pico 2)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-pico2.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-pico2.yml) |
| External Repo: Zephyr Reference (Teensy 4.1) | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | push, pull request | [![External Repo: Zephyr Reference (Teensy 4.1)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-teensy41.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-teensy41.yml) |
| External Repo: Generic Hub Reference | [`fprime-generic-hub-reference`](https://github.com/fprime-community/fprime-generic-hub-reference) | push, pull request | [![External Repo: Generic Hub Reference](https://github.com/nasa/fprime/actions/workflows/ext-generic-hub-reference.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-generic-hub-reference.yml) |
| Soak Setup: Pico 2 Zephyr Reference | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | on release, manual dispatch | [![Soak Setup: Pico 2 Zephyr Reference](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-setup.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-setup.yml) |
| Soak Test: Pico 2 Zephyr Reference | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | scheduled | [![Soak Test: Pico 2 Zephyr Reference](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-test.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-test.yml) |
| Soak Summary: Pico 2 Zephyr Reference | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | manual dispatch | [![Soak Summary: Pico 2 Zephyr Reference](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-summary.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-summary.yml) |
| External Repo: RPI LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | push, pull request | [![External Repo: RPI LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-raspberry-led-blinker.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-raspberry-led-blinker.yml) |
| External Repo: YAMCS Reference | [`fprime-yamcs-reference`](https://github.com/fprime-community/fprime-yamcs-reference) | push, pull request | [![External Repo: YAMCS Reference](https://github.com/nasa/fprime/actions/workflows/ext-yamcs-reference.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-yamcs-reference.yml) |
