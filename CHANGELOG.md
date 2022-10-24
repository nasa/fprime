### Release 3.0: Release and Migration Notes

Version 3.0.0 of F´ comes with several major enhancements to the framework. This release contains an update to use the FPP modeling language and the C++ standard has been updated to C++11. These are fairly substantial changes and users should consult the version 3 [migration guide](https://nasa.github.io/fprime/UsersGuide/user/v3-migration-guide.html) when adopting F´ version 3. 

### Release 2.1.0: Release Notes

This is the final release of the F´ version 2 releases. This should be the chosen release for projects unable to upgrade to C++11 and/or adopt FPP.

### Release 2.0.1: Release Notes

This is a point release that repaired some compilation issues with VxWorks 6.  This will be the last release supporting VxWorks 6.

### Release 2.0: Release and Migration Notes

Version 2.0.0 of F´ represents major improvements across the F´ framework. As such, some work may be required to migrate from other versions of F´ to the new
functionality. This section will offer recommendations to migrate to version 2.0.0 of F´.

Features and Functionality:

* New ground interface change improves stability and flexibility
  * `Svc::Framer` and `Svc::Deframer` components may be used in place of `Svc::GroundInterface`
  * `Svc::Framer` and `Svc::Deframer` delegate to a user instantiated framing class allowing use of non-fprime framing protocols
* `Drv::ByteStreamDriverModel` allows implementing drivers reading/writing streams of bytes using a single model
* New IPv4 drivers implement `Drv::ByteStreamDriverModel` allowing choice or combination of uplink and downlink communications
  * `Drv::TcpClient` is a tcp client that connects to a remote server
  * `Drv::TcpServer` is a tcp server that allows connections from remote clients
  * `Drv::Udp` allows UDP communications
  * `Drv::SocketIpDriver` may be replaced using a choice of an above component.
* `Svc::FileDownlink` now supports a queue of files to downlink and a port to trigger file downlinks
* `Svc::FileDownlink` may now be configured to turn off certain errors
* `Svc/GenericHub` is a basic instantiation of the hub pattern
* Bug fixes and stability improvements

Migration considerations:

* F´ tooling (fprime-util and fprime-gds) should be installed using `pip install fprime-tools fprime-gds`
* `Os::File::open` with the mode CREATE will now properly respect O_EXCL and error if the file exists. Pass in `false` as the final argument to override.
* Revise uses of `Fw::Buffer` to correct usage of member functions using camel case.  E.g. `Fw::Buffer::getsize` is now `Fw::Buffer::getSize`
* The ground interface chain has been refactored. Projects may switch to using `Svc::Framer`, `Svc::Deframer`, and any implementor of `Drv::ByteStreamDriverModel` to supply the data.  To continue using the old interface with the GDS run `fprime-gds --comm-checksum-type fixed`.
* `Svc::BufferManager` has been reworked to remove errors. When instantiating it please supply a memory allocator as shown in `Ref`.
* Dictionaries, binaries, and other build outputs now are written to a deployments `build_artifacts` folder.

**Deprecated Functionality:** The following features are or will be deprecated soon and may be removed in future releases.

* `Svc::GroundInterface` and `Drv::SocketIpDriver` should be replaced by the new ground system components.
* Inline enumerations (enumerations defined inside the definition of a command/event/channel) should be replaced by EnumAi.xml implementations
* `fprime-util generate --ut -DFPRIME_ENABLE_FRAMEWORK_UTS=OFF` will be removed in favor of future `fprime-util check` variants
* `Autocoders/MagicDrawCompPlugin` will be removed in a near-term release

### Release 1.5

* Documentation improvements
  * New user's guide containing considerable content: [https://nasa.github.io/fprime/UsersGuide/guide.html](https://nasa.github.io/fprime/UsersGuide/guide.html)
  * Auto-generated API documentation
  * Rewrites, edits, improvements across the board
* F´ Project restructuring
  * Projects may now link to F´ and F´ library packages, without needing to keep the framework code in the same source tree
  * Usage of framework can be out-of-source
  * `settings.ini` Introduced
  * Example: [https://github.com/fprime-community/fprime-arduino](https://github.com/fprime-community/fprime-arduino)
* Refactored `fprime-util`
  * Replaced redundant targets with flags e.g. build-ut is now build --ut
  * Added `info` command
  * Bug and usability fixes
* GDS Improvements
  * Prototype GDS CLI tool
  * Project custom dashboard support
* Array, Enum type support and examples
* Code linting and bug fixes

### Release 1.4

* Ref app no longer hangs on Linux exit
* GDS improvements:
  * File Uplink and Downlink implemented
  * GDS supports multiple active windows
  * Usability improvements for EVRs and commands
* CMake improvements:
  * Baremetal compilation supported
  * Random rebuilding fixed
  * Missing Cheetah templates properly rebuild
  * Separate projects supported without additional tweaks
* Updated MemAllocator to have:
  * "recoverable" flag to indicate if memory was recoverable across boots
  * size variable is now modifiable by allocator to indicate actual size
  * This will break existing code that uses MemAllocator
* Updated CmdSequencer
  * Uses new MemAllocator interface

### Release 1.3

* New prototype HTML GUI
* Python packages Fw/Python and Gds
* Refined CMake and fprime-util helper script
* Better ground interface component
* Integration test API
* Baremetal components

### Release 1.2

* Better MagicDraw Plugin
* Prototype CMake build system. See: [CMake Documentation](./docs/UsersGuide/cmake/cmake-intro.md)
* Mars Helicopter Project fixes migrated in
* Python 3 support added
* Gse refactored and renamed to Gds
* Wx frontend to Gds
* UdpSender and UdpReceiver components added
* Purged inaccurate ITAR and Copyright notices
* Misc. bug fixes

### Release 1.1

* Created a Raspberry Pi demo. Read about it [here](RPI/README.md)
* Added a tutorial [here](docs/Tutorials/README.md)
* Updated Svc/BufferManager with bug fix
* Fixed a bunch of shell permissions

### Release 1.0.1

* Updated contributor list. No code changes.

### Release 1.0

* This is the initial release of the software to open source. See the license file for terms of use.

