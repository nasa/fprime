# F Prime: A Flight-Proven, Multi-Platform, Open-Source Flight Software Framework

F Prime (FÊ¹) is a component-driven framework that enables rapid development and deployment of spaceflight and other embedded software applications. Originally developed at the Jet Propulsion Laboratory, F Prime has been successfully deployed on several space applications. It is tailored, but not limited, to small-scale spaceflight systems such as CubeSats, SmallSats, and instruments. 

F Prime comprises several elements: 

* An architecture that decomposes flight software into discrete components with well-defined interfaces
* A C++ framework that provides core capabilities such as message queues and threads
* Modeling tools for specifying components and connections and automatically generating code
* A growing collection of ready-to-use components
* Testing tools for testing flight software at the unit and integration levels.

F Prime has the following key features:

### Reusability

F Primeâ€™s component-based architecture enables a high degree of modularity and software reuse. 

### Rapid Deployment

F Prime provides a complete development ecosystem, including modeling tools, testing tools, and a ground data system. Developers use the modeling tools to write high-level specifications, automatically generate implementations in C++, and fill in the implementations with domain-specific code. The framework and the code generators provide all the boilerplate code required in an F Prime deployment, including code for thread management, code for communication between components, and code for handling commands, telemetry, and parameters. The testing tools and the ground data system simplify software testing, both on workstations and on flight hardware in the lab.

### Portability

F Prime runs on a wide range of processors, from microcontrollers to multicore computers, and on several operating systems. Porting F Prime to new operating systems is straightforward.

### High Performance

F Prime utilizes a point-to-point architecture. The architecture minimizes the use of computational resources and is well suited for smaller processors.
	
### Adaptability

F Prime is tailored to the level of complexity required for small missions. This makes it accessible and easy to use, while still supporting a wide variety of missions.

### Analyzability

The typed port connections provide strong compile-time guarantees of correctness.

## F Prime Resources

The Reference application is shipped as part of F'. Documentation for this reference application can be found [here.](Ref/docs/sdd.md)

The full F' User's guide can be found [here.](docs/UsersGuide/FprimeUserGuide.pdf) In addition, the F' architectural overview can be found [here.](docs/Architecture/FPrimeArchitectureShort.pdf)

## Continuous Integration

The continous integration system performs builds and unit-test check on any pull-requests created on the F´ core. Thus ensuring that F´ core is stable and well maintained.

## F Prime Release Notes

#### Release 1.0: 

 * This is the initial release of the software to open source. See the license file for terms of use.

#### Release 1.01

 * Updated contributor list. No code changes. 

#### Release 1.1

 * Created a Raspberry Pi demo. Read about it [here.](RPI/README.md)
 * Added a tutorial [here.](docs/Tutorials/README.md)
 * Updated Svc/BufferManager with bug fix
 * Fixed a bunch of shell permissions
 
#### Release 1.2

* Better MagicDraw Plugin
* Prototype CMake build system. See: [CMake Readme](cmake/README.md)
* Mars Helicopter Project fixes migrated in
* Python 3 support added
* Gse refactored and renamed to Gds
* Wx frontend to Gds
* UdpSender and UdpReceiver components added
* Purged inaccurate ITAR and Copyright notices
* Misc. bug fixes
