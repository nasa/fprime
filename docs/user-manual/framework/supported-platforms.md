# Supported Platforms

| Hardware         | OS | Architecture   | Reference Project |
| ---------------- | -------------------- | -------------- | ---------------------------------- |
| BeagleBone Black | VxWorks 7 | ARMv7 | [`fprime-vxworks-reference`](https://github.com/fprime-community/fprime-vxworks-reference) |
| Raspberry Pi     | Linux     | ARMv8 | [`fprime-workshop-led-blinker`](https://github.com/fprime-community/fprime-workshop-led-blinker) |
| x86              | Linux     | x86\_64 | [`F Prime Ref`](https://github.com/nasa/fprime) |
| Apple M*         | Darwin    | ARM  | [`F Prime Ref`](https://github.com/nasa/fprime) |

## Targeted Platforms (Planned Support)

These are platforms that the F Prime community or core team is actively working or planning to support. They may have partial implementations, early-stage deployments, or nothing yet.

| Hardware | OS        | Architecture | Status              | Reference Project             |
| -------- | --------- | ------------ | ------------------- | ---------------- |
| PyCubed  | Baremetal | RISC-V | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) |
| PyCubed  | Zephyr | RISC-V | In development ||
| 3PySquared | Baremetal  | ARM | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) |
| 3PySquared | Zephyr  | ARM | In development | |
| Pi Pico 2| Baremetal | RISC-V | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) |
| Pi Pico 2| Zephyr | RISC-V | In development ||
| Pi Pico  | Baremetal  | ARM | In development |[`fprime-baremetal-reference`](https://github.com/fprime-community/fprime-baremetal-reference) |
| Feather M4 | FreeRTOS  | ARM | In development  | |
| HPSC | VxWorks 7 | RISC-V | In development | |
| HPSC | Linux | RISC-V | In development | |
| PolarFire SoC  | VxWorks 7 | RISC-V | In development | |
| PolarFire SoC  | Linux | RISC-V | Not started | |
| GR712RC | VxWorks 7 | SPARC V8 | Not started | |
| Vorago | Baremetal | ARM | In development | |
| VOXL2 | Linux | ARM64 | In development | |

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
1. Optional: submit a request to add your reference project to fprime-community by [opening a discussion](https://github.com/nasa/fprime/discussions). Provide platform description in your request.
   
> [!TIP]
> 1. Follow the naming and file-structure convention as the example reference projects.
> 1. Avoid using hardcoded paths in your toolchain file. Use environment variables.
> 1. Test on actual hardware if possible—emulated environments may behave differently.
