# Supported Platforms

| Hardware         | OS | Architecture   | Reference Project | Build Status |
| ---------------- | -------------------- | ------------------------------- | ---------------------------------- | ------------ |
| Apple Silicon    | Darwin    | ARM  | [`F Prime Ref`](https://github.com/nasa/fprime) | [![CI [macOS]](https://github.com/nasa/fprime/actions/workflows/build-test-macos.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/build-test-macos.yml) |
| BeagleBone Black | VxWorks 7 | ARMv7 | [`fprime-vxworks-reference`](https://github.com/fprime-community/fprime-vxworks-reference) | N/A |
| Feather M4 	   | FreeRTOS  | ARM | [`fprime-featherm4-freertos-reference`](https://github.com/fprime-community/fprime-featherm4-freertos-reference) | N/A |
| Pi Pico          | Zephyr    | ARMv6-M | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | N/A |
| Pi Pico 2        | Zephyr    | RISC-V | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | [![External Repo: Zephyr Reference (Pico 2)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-pico2.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-pico2.yml) |
| PolarFire SoC    | VxWorks 7 | RISC-V | TBD | N/A |
| PyCubed          | Zephyr    | RISC-V | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | N/A |
| Raspberry Pi     | Linux     | ARMv8 | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | [![External Repo: RPI LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-raspberry-led-blinker.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-raspberry-led-blinker.yml) |
| Teensy41         | Zephyr    | ARMv7-M | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | [![External Repo: Zephyr Reference (Teensy 4.1)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-teensy41.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-teensy41.yml) |
| x86              | Linux     | x86\_64 | [`F Prime Ref`](https://github.com/nasa/fprime) | [![CI [ubuntu]](https://github.com/nasa/fprime/actions/workflows/build-test.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/build-test.yml) [![CI [RHEL8]](https://github.com/nasa/fprime/actions/workflows/build-test-rhel8.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/build-test-rhel8.yml) |

## Targeted Platforms (Planned Support)

These are platforms that the F Prime community or core team is actively working or planning to support. They may have partial implementations, early-stage deployments, or nothing yet.

| Hardware | OS        | Architecture | Status              | Reference Project             | Delivery Date |
| -------- | --------- | ------------ | ------------------- | ---------------- |----------------|
| 3PySquared | Baremetal  | ARM | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | TBD |
| 3PySquared | Zephyr  | ARM | In development | |TBD|
| GR712RC | VxWorks 7 | SPARC V8 | Not started | |TBD|
| GR740 | VxWorks 7 | SPARC V8 | Not started | |TBD|
| HPSC | Linux | RISC-V | In development | | TBD |
| HPSC | VxWorks 7 | RISC-V | In development | | TBD |
| Pi Pico  | Baremetal  | ARM | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | TBD |
| Pi Pico 2| Baremetal | RISC-V | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | TBD |
| PolarFire SoC  | Linux | RISC-V | Not started | |TBD|
| PyCubed  | Baremetal | RISC-V | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | TBD |
| Snapdragon CoProcessor | Linux | ARM | Not started | |TBD|
| Vorago | Baremetal | ARM | In development | |TBD|
| VOXL2 | Linux | ARM64 | In development | | TBD |

## Other References

The tables above list reference projects tied to a specific hardware and operating system pair. F Prime maintains further reference projects, tutorials, and examples that are not specific to a single platform.

### Other Reference Projects

| Reference Project | Description |
| ----------------- | ----------- |
| [`fprime_cfs_reference`](https://github.com/fprime-community/fprime_cfs_reference) | F Prime applications running under the Core Flight System (cFS) |
| [`fprime-amsat-main-board-reference`](https://github.com/fprime-community/fprime-amsat-main-board-reference) | Port to the Pico main sensor board for the AMSAT CubeSat Simulator |
| [`fprime-amsat-reference`](https://github.com/fprime-community/fprime-amsat-reference) | Port to the C&DH Pi Zero 2 board for the AMSAT CubeSat Simulator |
| [`fprime-generic-hub-reference`](https://github.com/fprime-community/fprime-generic-hub-reference) | Applications built around the Hub pattern |
| [`fprime-phased-deployment-reference`](https://github.com/fprime-community/fprime-phased-deployment-reference) | Deployment run entirely through phases |
| [`fprime-python-reference`](https://github.com/fprime-community/fprime-python-reference) | Reference for the `fprime-python` package |
| [`fprime-pycubed-baremetal-reference`](https://github.com/fprime-community/fprime-pycubed-baremetal-reference) | PyCubed baremetal deployment |
| [`fprime-pycubed-zephyr-reference`](https://github.com/fprime-community/fprime-pycubed-zephyr-reference) | PyCubed Zephyr deployment |
| [`fprime-sensors-reference`](https://github.com/fprime-community/fprime-sensors-reference) | Reference for the `fprime-sensors` library |
| [`fprime-stm32h7-zephyr-reference`](https://github.com/fprime-community/fprime-stm32h7-zephyr-reference) | STM32H7 Zephyr deployment |
| [`fprime-yamcs-reference`](https://github.com/fprime-community/fprime-yamcs-reference) | F Prime integrated with YAMCS mission control software |

### External Repository CI

F Prime runs continuous integration against external repositories so that reference projects and tutorials do not fall out of date as the framework changes. See [Automated Checks on Reference Repositories](https://github.com/nasa/fprime/blob/devel/CONTRIBUTING.md#automated-checks-on-reference-repositories) for how these checks relate to pull requests opened against F Prime.

| Workflow | Project Under Test | Trigger | Status |
| -------- | ------------------ | ------- | ------ |
| External Repo: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | Push / PR | [![External Repo: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker.yml) |
| External Repo: Cookiecutters Tests | [`fprime-bootstrap`](https://github.com/fprime-community/fprime-bootstrap), [`fprime-tools`](https://github.com/nasa/fprime-tools) | Push / PR | [![External Repo: Cookiecutters Tests](https://github.com/nasa/fprime/actions/workflows/ext-cookiecutters-test.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-cookiecutters-test.yml) |
| External Repo: RPI LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | Push / PR | [![External Repo: RPI LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-raspberry-led-blinker.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-raspberry-led-blinker.yml) |
| External Repo: Tutorial: HelloWorld | [`fprime-tutorial-hello-world`](https://github.com/fprime-community/fprime-tutorial-hello-world) | Push / PR | [![External Repo: Tutorial: HelloWorld](https://github.com/nasa/fprime/actions/workflows/ext-build-hello-world.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-hello-world.yml) |
| External Repo: Tutorial: LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | Push / PR | [![External Repo: Tutorial: LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-build-led-blinker.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-led-blinker.yml) |
| External Repo: Tutorial: MathComponent | [`fprime-tutorial-math-component`](https://github.com/fprime-community/fprime-tutorial-math-component) | Push / PR | [![External Repo: Tutorial: MathComponent](https://github.com/nasa/fprime/actions/workflows/ext-build-math-comp.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-math-comp.yml) |
| External Repo: YAMCS Reference | [`fprime-yamcs-reference`](https://github.com/fprime-community/fprime-yamcs-reference) | Push / PR / manual | [![External Repo: YAMCS Reference](https://github.com/nasa/fprime/actions/workflows/ext-yamcs-reference.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-yamcs-reference.yml) |
| External Repo: Zephyr Reference (Pico 2) | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | Push / PR | [![External Repo: Zephyr Reference (Pico 2)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-pico2.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-pico2.yml) |
| External Repo: Zephyr Reference (Teensy 4.1) | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | Push / PR | [![External Repo: Zephyr Reference (Teensy 4.1)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-teensy41.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-zephyr-reference-teensy41.yml) |
| External Repo: cFS Reference | [`fprime_cfs_reference`](https://github.com/fprime-community/fprime_cfs_reference) | Push / PR | [![External Repo: cFS Reference](https://github.com/nasa/fprime/actions/workflows/ext-fprime-cfs-reference.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-fprime-cfs-reference.yml) |
| External Repo: fprime-examples | [`fprime-examples`](https://github.com/nasa/fprime-examples) | Push / PR | [![External Repo: fprime-examples](https://github.com/nasa/fprime/actions/workflows/ext-build-examples-repo.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-build-examples-repo.yml) |
| Soak Setup: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | Release / manual | [![Soak Setup: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-setup.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-setup.yml) |
| Soak Setup: Pico 2 Zephyr Reference | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | Release / manual | [![Soak Setup: Pico 2 Zephyr Reference](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-setup.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-setup.yml) |
| Soak Summary: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | Manual | [![Soak Summary: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-summary.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-summary.yml) |
| Soak Summary: Pico 2 Zephyr Reference | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | Manual | [![Soak Summary: Pico 2 Zephyr Reference](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-summary.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-summary.yml) |
| Soak Test: AArch64 Linux LedBlinker | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) | Scheduled / manual | [![Soak Test: AArch64 Linux LedBlinker](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-test.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-aarch64-linux-led-blinker-soak-test.yml) |
| Soak Test: Pico 2 Zephyr Reference | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) | Scheduled / manual | [![Soak Test: Pico 2 Zephyr Reference](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-test.yml/badge.svg)](https://github.com/nasa/fprime/actions/workflows/ext-pico2-zephyr-reference-soak-test.yml) |

## What does "Supported Platform" Mean?

A **platform** refers to a combination of hardware and operating system (OS). A **supported platform** satisfies the following criteria:

1. Reference project has been delivered to F Prime maintainers.
1. Platform library exists and is used as part of a reference application. The platform library provides OSAL implementation, cmake support, drivers, and anything else needed to build and run the Reference project.
1. Tested continuously as part of a continuous integration (CI) pipeline (e.g., unit tests, integration tests).
1. Actively maintained by the F Prime community or core F Prime team.
1. Documented build and run instructions are available for users.

## How to Contribute a New Supported Platform

We welcome contributions to expand the list of supported platforms! Here’s how to get started:

1. Pick a hardware and OS platform that is not in development or is not listed.
1. Create a platform library. Take a look at [`How-To: Develop an F Prime Library`](https://fprime.jpl.nasa.gov/latest/docs/how-to/develop-fprime-libraries/).
1. Create a reference project for your platform that uses your platform library. Use [`fprime-vxworks-reference`](https://github.com/fprime-community/fprime-vxworks-reference) or [`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) as examples for the file structure.
1. Include build, run, and hardware setup instructions in a README.md.
1. Set up CI for your reference project and have CI run the F Prime provided integration test scripts.
1. Optional: You may submit a request to transfer your reference project to the fprime-community by [opening a discussion](https://github.com/nasa/fprime/discussions). Please include a description of your platform and links to your reference project repositories. To transfer a repository to fprime-community, you need to transfer ownership of the repository to an fprime maintainer. If accepted, then they will transfer the repository to the fprime-community organization.
   
> [!TIP]
> 1. Follow the naming and file-structure convention as the example reference projects.
> 1. Avoid using hardcoded paths in your toolchain file. Use environment variables.
> 1. Test on actual hardware if possible—emulated environments may behave differently.
