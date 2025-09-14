# Supported Platforms

| Hardware         | OS | Architecture   | Reference Project |
| ---------------- | -------------------- | -------------- | ---------------------------------- |
| Apple Silicon    | Darwin    | ARM  | [`F Prime Ref`](https://github.com/nasa/fprime) |
| BeagleBone Black | VxWorks 7 | ARMv7 | [`fprime-vxworks-reference`](https://github.com/fprime-community/fprime-vxworks-reference) |
| Pi Pico          | Zephyr    | ARMv6-M | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) |
| Pi Pico 2        | Zephyr    | RISC-V | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) |
| PolarFire SoC    | VxWorks 7 | RISC-V | TBD |
| PyCubed          | Zephyr    | RISC-V | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) |
| Raspberry Pi     | Linux     | ARMv8 | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) |
| Teensy41         | Zephyr    | ARMv7-M | [`fprime-zephyr-reference`](https://github.com/fprime-community/fprime-zephyr-reference) |
| x86              | Linux     | x86\_64 | [`F Prime Ref`](https://github.com/nasa/fprime) |

## Targeted Platforms (Planned Support)

These are platforms that the F Prime community or core team is actively working or planning to support. They may have partial implementations, early-stage deployments, or nothing yet.

| Hardware | OS        | Architecture | Status              | Reference Project             | Delivery Date |
| -------- | --------- | ------------ | ------------------- | ---------------- |----------------|
| 3PySquared | Baremetal  | ARM | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | 9/30/2025 |
| 3PySquared | Zephyr  | ARM | In development | |TBD|
| Feather M4 | FreeRTOS  | ARM | In development  |[`fprime-featherm4-freertos-reference`](https://github.com/fprime-community/fprime-featherm4-freertos-reference) | 9/30/2025 |
| GR712RC | VxWorks 7 | SPARC V8 | Not started | |TBD|
| GR740 | VxWorks 7 | SPARC V8 | Not started | |TBD|
| HPSC | Linux | RISC-V | In development | | 9/30/2025 |
| HPSC | VxWorks 7 | RISC-V | In development | | 9/30/2025 |
| Pi Pico  | Baremetal  | ARM | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | 9/30/2025 |
| Pi Pico 2| Baremetal | RISC-V | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | 9/30/2025 |
| PolarFire SoC  | Linux | RISC-V | Not started | |TBD|
| PyCubed  | Baremetal | RISC-V | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) | 9/30/2025 |
| Snapdragon CoProcessor | Linux | ARM | Not started | |TBD|
| Vorago | Baremetal | ARM | In development | |TBD|
| VOXL2 | Linux | ARM64 | In development | | 9/30/2025 |

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
