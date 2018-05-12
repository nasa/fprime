---
title: Getting Started
layout: default
---
# Getting Started with F'

In this guide, we will cover the basics of working with F'. To start with, we will grab the code, build the reference application, and verify
that the reference application will run. After that we will create a custom F' component and build it into out own application. Along the way
we will cover: Components, Topologies, Events, Telemetry, and Commands. In addition, we will see how to modify the build-system to include our
new components and topologies.

## Getting Started with F' and the Reference Application

The first step to running F' is to ensure that the required build tools are available on your system. At its most basic, F' requires
three tools to be installed: Git, GNU make, and python w/ cheetah. Make is available on most systems, and cheetah can be installed in python
with the following command:

```
pip install cheetah
```

### Cloning the Software and Building the Reference Application

The next step is to clone the software from GitHub. The following command will clone the software straight from NASA's GitHub project page.

```
git clone https://github.com/nasa/fprime.git
```

Once the software is cloned, the reference-application can be built by changing into the reference directory, generating make files,
clean and makeing the code.

```
cd fprime/Ref
make gen_make
make clean
make
```

If the build progresses to success, the reference application has been successfully built. The binary outputs can be found in a new directory named with
the architecture for which you built the reference application. i.e. *darwin-darwin-x86-debug-llvm-bin* or *linux-linux-x86-debug-gnu-bin*

### Running the Reference Application

In order to run the reference application, one needs to run the binary named *Ref* in the above created target directory. Running the application will 
show that the reference application was successfully built and runnable. Replace *darwin-darwin-x86-debug-llvm-bin* in the below commands by the target
used in building the reference application

**On OS X**:
```
cd fprime/Ref/darwin-darwin-x86-debug-llvm-bin
./Ref
```

**On Linux**:
```
cd fprime/Ref/linux-linux-x86-debug-gnu-bin
./Ref
```

Success!!! We have now successfully run the reference application, showing that the build tools work. We are ready to proceed to a custom application.

## Custom Application Goals

In the DIY electronics community there is an abundace of cheap GPS receivers based around the NEMA protocol. These receivers often suport a USB interface
pretending to be am ACM device for basic serial communication. The messages these receivers send are NEMA formatted text.

TODO: add photo here

This quick-start guide will show how to integrate one of these GPS receivers with the F' framework by wrapping it in a Component and defining commands,
telemetry, and log events. Finnaly, we will modify the reference Topology to include this new component such that we can downlink our telemetry to
the F' supplied ground station (GSE).


## Creating a Custom F' Component

In this next section we will create a custom F' component for reading GPS data off a UART based GPS module. We will then downlink this data as telemetry
and finish up by adding an event to report GPS lock status and a command to report lock-status on demand.

### Designing the GPS Component

Currently, F' designs are stored in XML files that feed the auto-coding system. An XML file represents a signle component including the ports it uses
to communicate with other components as well as references to the dictonaries that define events/log messages, telmetry, and commands. Further discussion
of components, ports, events, telemetry, and commands can be found in the F' user guide. [User Guide](docs/UsersGuide/FprimeUserGuide.pdf). *Side note:* 
Ports are also defined with XML, however; this application does not need any customized ports and thus we need not elaborate here.

The first step to making a compnent is to make a project directory to hold our project's components, and a component directory for our GPS. This is do
as follows:

```
cd fprime
mkdir -p GpsApp/Gps
cd GpsApp/Gps
```

Next, in this directory, we will create a file called *GpsComponentAi.xml* filled with the below text. This represents our Components design by defining
the ports it uses to connect with other components and the files used to specify commands, telemetry, and events. As can be seen, we are creating our component
to have 5 ports:

#. **cmdRegOut**: an output port of *Fw::CmdReg* type used to register this component's commands with the command dispatcher
#. **eventOut**: an output port of *Fw::Log* type used to log events
#. **cmdIn**: an input port of *Fw::Cmd* type used to process commands sent to this component
#. **tlmOut**: an output port of *Fw::Tlm* type used to send out telemetry items
#. **cmdResponseOut**: an output port of *Fw::CmdResponse* type used to signal a command has finished


In addition, our component imports the above port types, and imports 3 dictionaries: Commands.xml, Events.xml, and Telemetry.xml. The GpsComponentAi.xml
file should look like this:

```
<?xml version="1.0" encoding="UTF-8"?>
<?xml-model href="../../Autocoders/schema/ISF/component_schema.rng" type="application/xml" schematypens="http://relaxng.org/ns/structure/1.0"?>

<component name="Gps" kind="active" namespace="GpsApp" modeler="true">

    <import_port_type>Fw/Cmd/CmdRegPortAi.xml</import_port_type>
    <import_port_type>Fw/Log/LogPortAi.xml</import_port_type>
    <import_port_type>Fw/Cmd/CmdPortAi.xml</import_port_type>
    <import_port_type>Svc/Sched/SchedPortAi.xml</import_port_type>
    <import_port_type>Fw/Tlm/TlmPortAi.xml</import_port_type>
    <import_port_type>Fw/Cmd/CmdResponsePortAi.xml</import_port_type>
    <import_dictionary>Svc/FileDownlink/Commands.xml</import_dictionary>
    <import_dictionary>Svc/FileDownlink/Telemetry.xml</import_dictionary>
    <import_dictionary>Svc/FileDownlink/Events.xml</import_dictionary>
    <ports>


        <port name="cmdRegOut" data_type="Fw::CmdReg"  kind="output" role="CmdRegistration"    max_number="1">
        </port>

        <port name="eventOut" data_type="Fw::Log"  kind="output" role="LogEvent"    max_number="1">
        </port>

        <port name="cmdIn" data_type="Fw::Cmd"  kind="input" role="Cmd"    max_number="1">
        </port>

        <port name="schedIn" data_type="Svc::Sched"  kind="async_input"    max_number="1">
        </port>

        <port name="tlmOut" data_type="Fw::Tlm"  kind="output" role="Telemetry"    max_number="1">
        </port>

        <port name="cmdResponseOut" data_type="Fw::CmdResponse"  kind="output" role="CmdResponse"    max_number="1">
        </port>
    </ports>

</component>
```

Connecting these components together is done at the system level, enabling the individual components to be reused in different applications. We will
see this step later.

## Creating Commands.xml, Events.xml and Telemetry.xml Dictionaries

These three XML dictionaries define the structure of commands, events, and telemetry that our component uses. This allows the auto-coding system
to automatically generate the needed code to process commands, and emit events and telemetry. This save the developer for writing the specific code
to drive the component.

First we will create a command dictionary. The purpose of our command is to report the lock status of the GPS unit. This command will trigger an event
which will report if the GPS is locked or not. Command.xml should look like the following:

```
<?xml version="1.0" encoding="UTF-8"?>
<?oxygen RNGSchema="file:../xml/ISF_Component_Schema.rnc" type="compact"?>

<!--====================================================================== 

  GpsApp/Gps/Commands.xml

======================================================================-->

<commands
  opcode_base="$GpsIdOpcodeBase"
>

  <command
    kind="async"
    opcode="0"
    mnemonic="Gps_ReportLockStatus"
  >
    <comment>A command to force an EVR reporting lock status.</comment>
  </command>

</commands>
```

**Note:** opcode_base is auto-filled by the auto-coder giving out component's opcodes a unique offset so that we need not worry about what op-codes are
defined outside this component. We can just number them within the component, and expect the auto-coder to resolve collisions by prefixing the op-codes
with a defined offset.

Next we will create an Events.xml dictionary that setup the events our component can log. In this case we have two events, GPS locked and GPS lock lost.
Here again we have and ID base allowing us to fill in the event offsets later. The Events.xml file should look like:

```
<?xml version="1.0" encoding="UTF-8"?>
<?oxygen RNGSchema="file:../xml/ISF_Component_Schema.rnc" type="compact"?>

<!--====================================================================== 

  GpsApp/Gps/Events.xml

======================================================================-->

<events
  event_base="$GpsIdEventsBase"
>

  <event
    id="0"
    name="Gps_LockAquired"
    severity="ACTIVITY_HI"
    format_string="GPS lock acquired"
  >
    <comment>A notification on GPS lock aquired</comment>
  </event>

  <event
    id="1"
    name="Gps_LockLost"
    severity="WARNING_HI"
    format_string="GPS lock lost"
  >
    <comment>A notification on GPS lock lost</comment>
  </event>

</events>
```

Finally, we should create a Telemetry.xml dictionary. It will specify that we will downlink latitude, longitude, altitude, time, and current number of 
satellites visible to the GPS unit. These are all standard fields emitted by a UART based GPS unit. Our file should look like:

```
<?xml version="1.0" encoding="UTF-8"?>
<?oxygen RNGSchema="file:../xml/ISF_Component_Schema.rnc" type="compact"?>

<!--====================================================================== 

  GpsApp/Gps/Telemetry

======================================================================-->

<telemetry
  telemetry_base="$GpsIdTelemetryBase"
>

  <channel
    id="0"
    name="Gps_Latitude"
    data_type="F32"
    abbrev="GPS-0000"
  >
    <comment>The current latitude</comment>
  </channel>

  <channel
    id="1"
    name="Gps_Longitude"
    data_type="F32"
    abbrev="GPS-0001"
  >
    <comment>The current longitude</comment>
  </channel>

  <channel
    id="2"
    name="Gps_Altitude"
    data_type="F32"
    abbrev="GPS-0002"
  >
    <comment>The current altitude</comment>
  </channel>

  <channel
    id="3"
    name="Gps_Count"
    data_type="U32"
    abbrev="GPS-0003"
  >
    <comment>The current number of satilites</comment>
  </channel>

</telemetry>
```

Now our dictionaries are setup. Before we can continue, we need to add *GpsIdBase* into the auto-coder constants file located at *fprime/Fw/Cfg/AcConstants.ini*. The end of this file should look like the following. This is the offset we disscussed earlier. In order to prevent collisions, it should be unique for other base ids set in
this file.

```
...
#
# Demo application components
#
# Base IDs
GpsIdBase                           =       0x0000                   ; Starting range for GPS IDs
# Define starting opcodes
GpsIdOpcodeBase                     =       %(GpsIdBase)s            ; Starting range for GPS IDs
# Define starting telemetry IDs
GpsIdTelemetryBase                  =       %(GpsIdBase)s            ; Starting telemetry ID for GPS
# Define starting event IDs
GpsIdEventsBase                     =       %(GpsIdBase)s            ; Starting Event ID for GPS
```

## Setting Up the Build System for Gps and GpsApp

The make system is configured to use *fprime/mk/configs/modules/modules.mk* as a place for specifying builds and various deployments. If we look
into this file, we can see REF_ and Ref_ setups for the Reference applications. We can replicate this for our GpsApp by adding the following lines
to the file right before the Autocoder sections.

```
...
GPS_MODULES := \
    GpsApp/Top \
    GpsApp/Gps
    
GpsApp_MODULES := \
    \
    $(GPS_MODULES) \
    \
    $(SVC_MODULES) \
    \
    $(DRV_MODULES) \
    \
    $(FW_MODULES) \
    \
    $(OS_MODULES) \
    \
    $(CFDP_MODULES) \
    \
    $(UTILS_MODULES)
...
```
This adds our (future) topology to the make system as well as the component we just created. This allows us to generate make files, and make our component. In
addition, we will be able to build our project's topology one we add that. *Note:* since we already added a folder for our Topology to the make system we'll need
to create a placeholder folder (below) before we can build.

In addition, the line defining *DEPLOYMENTS :=* should be modified to add GpsApp like the following:

```
DEPLOYMENTS := Ref GpsApp acdev
```

Given that we have already defined our *Topology* folder in the *GpsApp/Top* line above, we will need to add a placeholder for the Topology of our
GpsApp, which we will fill later. Make the folder *fprime/GpsApp/Top* and add an empty file *mod.mk*. We will fill this folder and mod.mk file
lateri, but creating it here enables us to build our component. Now our make system should be ready for us to build our new module, and take advantage of
 the code-generation that is available.

## Setting Up Module Make Files and Code

The first thing to do is to setup the Gps components's make files. Each module (component, port, topology) has two files: a Makefile and a mod.mk file. The makefile contains the module name and little else. The mod.mk file contains a list of all the files and submodules that makeup this module. We should now create the two file, *Makefile* and *mod.mk* using the following content in the *fprime/GpsApp/Gps* folder.

The makefile, called *Makefile*, looks like:
```
# ---------------------------------------------------------------------- 
# Makefile
# ---------------------------------------------------------------------- 

MODULE_DIR = GpsApp/Gps
MODULE = $(subst /,,$(MODULE_DIR))

BUILD_ROOT ?= $(subst /$(MODULE_DIR),,$(CURDIR))
export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/module_targets.mk
```
Apart from the *MODULE_DIR* variable, most Makefiles look the same.

The mod.mk file, called *mod.mk*, looks like:
```
# ---------------------------------------------------------------------- 
# mod.mk
# ---------------------------------------------------------------------- 

SRC = \
    GpsComponentAi.xml
    
#        GpsComponentImpl.cpp

HDR = \

#        GpsComponentImpl.hpp

#SUBDIRS = test
```
*Note:* since we have not created the code files yet, we leave them commented out. This allows us to test the make commands, without erring for
missing files. We can test the make system by running the following commands. *make gen_make* will auto-generate parts of the make system, *make 
clean* cleans the module, and *make* build the module.

```
make gen_make
make clean
make
```
If these commands pass without errors, we are ready to start coding our module.

## Coding Our Module

Now it is time to code our module to read the GPS module and downlink the GPS telemetry. This is where the framework will help us considerably. All 
these previous steps set us up to use the auto-coding features of F'. We can generate the basic implementation of the code by using the make 
command *make impl*, which generates needed *GpsComponentImpl.cpp-tmpl* and *GpsComponentImpl.hpp-tmpl* files. We can use these as bases for our 
implementation. In adition, the framework will also generate * *Ac.?pp* files, which handle the work of connecting ports allowing us to write 
minimal code to support the component interface. Below we will take a look. First we generate code templates, and move them into place (because 
don't already have implementations we can safely rename the template files).

```
make impl
mv GpsComponentImpl.cpp-tmpl GpsComponentImpl.cpp
mv GpsComponentImpl.hpp-tmpl GpsComponentImpl.hpp
```

*Note:* if the developer previously created the implementation .cpp and .hpp files for out component, then moving the templates in place could
be dangerous.

These template file contents are in the following sections.

### GpsComponentImpl.hpp
```
#ifndef Gps_HPP
#define Gps_HPP

#include "GpsApp/Gps/GpsComponentAc.hpp"

namespace GpsApp {

  class GpsComponentImpl :
    public GpsComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Gps
      //!
      GpsComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Gps
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Gps
      //!
      ~GpsComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for Gps_ReportLockStatus command handler
      //! A command to force an EVR reporting lock status.
      void Gps_ReportLockStatus_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );


    };

} // end namespace GpsApp

#endif
```

### GpsComponentImpl.cpp
```
#include <GpsApp/Gps/GpsComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace GpsApp {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  GpsComponentImpl ::
#if FW_OBJECT_NAMES == 1
    GpsComponentImpl(
        const char *const compName
    ) :
      GpsComponentBase(compName)
#else
    GpsImpl(void)
#endif
  {

  }

  void GpsComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    GpsComponentBase::init(queueDepth, instance);
  }

  GpsComponentImpl ::
    ~GpsComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void GpsComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void GpsComponentImpl ::
    Gps_ReportLockStatus_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
  }

} // end namespace GpsApp
```

### Implementation

Here it can be seen that we have two actions "TODO". First we will need to implement a function called *schedIn_handler* and
the second is to implement a command handler for *Gps_ReportLockStatus_cmdHandler*. Both are port calls that will be made
into our component. Ports are found in the GpsComponentAi.xml, however; only our *schedIn* port is used directly. The 
rest are called as part of telemetry, event logging, or command processing and thus have helpful wrappers (seen below).
schedIn represents a call from a rate group driver. It is called a a set rate, e.g. 10Hz based on the rate group to which it 
is connected. We will use this schedIn call to read the UART interface at a set rate of 1HZ. *Gps_ReportLockStatus_cmdHandler* is a 
handler that implements the function for the command we defined in Command.xml. We also have the following out-going port calls, 
and events that can be used as part of our code. 

#. log_ACTIVITY_HI_Gps_LockAquired: used to emit the event *Gps_Lock_aquired* as defined in Events.xml
#. log_WARNING_HI_Gps_LockLost: used to emit the event *Gps_LockLost* as defined in Events.xml
#. tlmWrite_Gps_Latitude: used to send down *Latitude* telemetry as defined in Telemetry.xml
#. tlmWrite_Gps_Longitude: used to send down *Longitude* telemetry as defined in Telemetry.xml
#. tlmWrite_Gps_Altitude: used to send down *Altitude* telemetry as defined in Telemetry.xml
#. tlmWrite_Gps_Count: used to send down *Count* telemetry as defined in Telemetry.xml
#. sendCommandResponse: used to respond to a sent command. Call this in the above cmdHandler.

In order to make a GPS processor, we need to take the following steps:

0. Implement the schedIn function (called at 1HZ)
1. (Re)initialize the UART in the call to schedIn
2. Read UART in schedIn
3. Parse UART data in schedIn
4. Downlink telmetry in schedIn
5. If lock is newly found, downlink a *LockAquired* EVR
6. If lock is newly lost, downlink a *LockLost* EVR
7. Downlink a *LockAquired* EVR in commandHandler, if lock is currently held
8. Downlink a *LockLost* EVR in commandHandler, if lock is not currently held
9. Respond from commandHandler with a sendCommandResponse call

These steps are called out in the following implementations of these two files. Since the purpose of the getting-started tutorial is not to demonstrate
how to write each line of code, the steps above are called out in comments in the code.

### GpsComponentImpl.cpp (Sample)
```
#include <GpsApp/Gps/GpsComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

//File path to UART device on UNIX system
#define UART_FILE_PATH "/dev/ttyACM0"
#include <cstring>
#include <iostream>
//POSIX includes for UART communication
extern "C" {
    #include <unistd.h>
    #include <fcntl.h>
    #include <termios.h>
}

namespace GpsApp {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  GpsComponentImpl ::
#if FW_OBJECT_NAMES == 1
    GpsComponentImpl(
        const char *const compName
    ) :
      GpsComponentBase(compName),
#else
      GpsComponentBase(void),
#endif
      m_setup(false),
      m_locked(false),
      m_fh(-1)
  {

  }

  void GpsComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    GpsComponentBase::init(queueDepth, instance);
  }

  GpsComponentImpl ::
    ~GpsComponentImpl(void)
  {

  }

  //Step 1: setup
  //
  // Each second, we should ensure that the UART is initialized
  // and if not, we should try to initialize it again.
  void GpsComponentImpl ::
    setup(void)
  {
      if (m_setup) {
          return;
      }
      //Open the GPS, and configure it for a raw-socket in read-only mode
      m_fh = open(UART_FILE_PATH, O_RDONLY);
      if (m_fh < 0) {
          std::cout << "[ERROR] Failed to open file: " << UART_FILE_PATH << std::endl;
          return;
      }
      //Setup the struct for termios configuration
      struct termios options;
      std::memset(&options, 0, sizeof(options));
      //Set to raw socket, remove modem control, allow input
      cfmakeraw(&options);
      options.c_cflag |= (CLOCAL | CREAD);
      //Set to 9600 baud
      cfsetispeed(&options, B9600);
      //Make the above options stick
      NATIVE_INT_TYPE status = tcsetattr(m_fh, TCSANOW, &options);
      if (status != 0) {
          std::cout << "[ERROR] Failed to setup UART options" << std::endl;
          return;
      }
      m_setup = true;
  }
  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  // Step 0: schedIn
  //
  //  By implementing this "handler" we can respond to the 1HZ call allowing
  //  us to read the GPS UART every 1 second.
  void GpsComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      int status = 0;
      float lat = 0.0f, lon = 0.0f;
      GpsPacket packet;
      char buffer[1024];
      char* pointer = buffer;
      //During each cycle, attempt to setup if not setup
      //Step 1: setup
      // Each second, we should ensure that the UART is initialized
      // and if not, we should try to initialize it again.
      setup();
      if (!m_setup) {
          return;
      }
      //Then receive data from the GPS. Should block until available
      //and thus, this module should not be driven at a rate faster than 1HZ
      //Step 2: read the UART
      // Read the GPS message from the UART
      ssize_t size = read(m_fh, buffer, sizeof(buffer));
      if (size <= 0) {
          std::cout << "[ERROR] Failed to read from UART with: " << size << std::endl;
          return;
      }
      //Look for a recognized GPS location packet and parse it
      //Step 3:
      // Parse the GPS message from the UART (looking for $GPPA messages)
      for (U32 i = 0; i < sizeof(buffer) - 6; i++) {
          status = sscanf(pointer, "$GPGGA,%f,%f,%c,%f,%c,%u,%u,%f,%f",
              &packet.utcTime, &packet.dmNS, &packet.northSouth,
              &packet.dmEW, &packet.eastWest, &packet.lock,
              &packet.count, &packet.filler2, &packet.altitude);
          //Break when all GPS items are found
          if (status == 9) {
              break;
          }
          pointer = pointer + 1;
      }
      //If we failed to find the packet, or failed to extract data then return
      if (status != 9) {
          std::cout << "[ERROR] GPS parsing failed: " << status << std::endl;
          return;
      }
      //GPS packet locations are of the form: dd.mmmm
      //We will convert to lat/lon in degrees only before downlinking
      lat = ((packet.northSouth == 'N') ? 1 : -1) *
            ((packet.dmNS - (U32)packet.dmNS)/60.0 + (U32)packet.dmNS);
      lon = ((packet.eastWest == 'E') ? 1 : -1) *
            ((packet.dmEW - (U32)packet.dmEW)/60.0 + (U32)packet.dmEW);
      //Step 4: downlink
      // Call the downlink functions to send down data
      tlmWrite_Gps_Latitude(lat);
      tlmWrite_Gps_Longitude(lon);
      tlmWrite_Gps_Altitude(packet.altitude);
      tlmWrite_Gps_Count(packet.count);
      //Lock status update only if changed
      //Step 7,8: note changed lock status
      // Emit an event if the lock has been aquired, or lost
      if (packet.lock == 0 && m_locked) {
          m_locked = false;
          log_WARNING_HI_Gps_LockLost();
      } else if (packet.lock == 1 && !m_locked) {
          m_locked = true;
          log_ACTIVITY_HI_Gps_LockAquired();
      }
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------
  //Step 9: respond to status command
  //
  // When a status command is received, respond by emitting the 
  // current lock status as an Event.
  void GpsComponentImpl ::
    Gps_ReportLockStatus_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    //Locked-force print
    if (m_locked) {
        log_ACTIVITY_HI_Gps_LockAquired();
    } else {
        log_WARNING_HI_Gps_LockLost();
    }
  }
} // end namespace GpsApp
```
### GpsComponentImpl.hpp (Sample)
```
#ifndef Gps_HPP
#define Gps_HPP

#include "GpsApp/Gps/GpsComponentAc.hpp"

namespace GpsApp {

  class GpsComponentImpl :
    public GpsComponentBase
  {
      /**
       * GpsPacket:
       *   A structure containing the information in the GPS location pacaket
       * received via the NEMA GPS receiver.
       */
      struct GpsPacket {
          float utcTime;
          float dmNS;
          char northSouth;
          float dmEW;
          char eastWest;
          unsigned int lock;
          unsigned int count;
          float filler;
          float altitude;
      };
    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Gps
      //!
      GpsComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Gps
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Gps
      //!
      ~GpsComponentImpl(void);

    PRIVATE:
      //! Setup the UART interface for taking with the GPS module. Note: this
      //! is currently implemented using standard Unix /dev/tty* devices.
      //!
      void setup(void);
      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for Gps_ReportLockStatus command handler
      //! A command to force an EVR reporting lock status.
      void Gps_ReportLockStatus_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );
      //!< Is the GPS UART setup
      bool m_setup;
      //!< Has the device acquired GPS lock?
      bool m_locked;
      //!< File handle of UART
      NATIVE_INT_TYPE m_fh;
    };

} // end namespace GpsApp

#endif
```

Finally, regenerate the makefiles, clean, and build. We will use this pattern a lot.
```
make gen_make
make clean
make
```

We are now ready to make a Topology for this application, and test it! 

## Topology

We are finally ready to build our topology to connect the GPS module up to the rate converter, and logger. Then we can see
if this will work. Generally, this process is done with a design tool like MagicDraw, but because the user may or may not
have access to this tool, the topology Ai is provided below. The user may then edit it to make changes.

First, the Reference application's topology is copied into the *Top* folder of *GpsApp* application and we'll grab the deployment makefile 
too.

```
cp -r fprime/Ref/Top fprime/GpsApp/Top
cp fprime/Ref/Makefile fprimt/GpsApp
```

Then update *fprime/GpsApp/Makefile* to set the following line to our deployment's name *GpsApp*.

```
DEPLOYMENT := GpsApp
```










