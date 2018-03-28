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
two tools to be installed: Git, GNU make, and python w/ cheetah. Make is available on most systems, and cheetah can be installed in python
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
the architecture for which you built the reference application. *i.e. darwin-darwin-x86-debug-llvm-bin*

### Running the Reference Application

In order to run the reference application, one needs to run the binary named *Ref* in the above created target directory. Running the application will 
show that the reference application was successfully built and runnable. Replace *darwin-darwin-x86-debug-llvm-bin* in the below commands by the target
used in building the reference application

```
cd fprime/Ref/darwin-darwin-x86-debug-llvm-bin
./Ref
```

Success!!!

## Creating a Custom F' Component

In this next section we will create a custom F' component for reading GPS data off a UART based GPS module. We will then downlink this data as telemetry
and finish up by adding an event to report GPS lock status and a command to report lock-status on demand.

### Designing the GPS Component

Currently, F' designs are stored in XML files that feed the auto-coding system. An XML file represents a signle component including the ports it uses
to communicate with other components as well as references to the dictonaries that define events/log messages, telmetry, and commands. Further discussion
of components, ports, events, telemetry, and commands can be found in the F' user guide. [User Guide].

We will make a custom directory to hold our components, and a component directory:

```
cd fprime
mkdir -p GpsApp/Gps
cd GpsApp/Gps
```

Next in this directory, we will create a file called *GpsComponentAi.xml* filled with the following text. As can be seen, we are creating our component
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

## Creating Commands.xml, Events.xml and Telemetry.xml Dictionaries

First we will create a command dictionary. The purpose of the command is to report the lock status of the GPS unit. This command will trigger an event
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

**Note:** opcode_base is auto-filled by the auto-coder giving out component's opcodes a unique offset so all our opcode values can be numbered starting
at zero.

Next we will create an Events.xml dictionary that setup the events our component can log. In this case we have two events, GPS locked and GPS lock lost.
Here again we have and ID base allowing us to fill in the offsets later. The Events.xml file should look like:

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

Now our dictionaries are setup. Before we can continue, we need to add *GpsIdBase* into the auto-coder constants file located at *fprime/Fw/Cfg/AcConstants.ini*. The end of this file should look like the following:

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

The make system is configured to used *fprime/mk/configs/modules/modules.mk* as a place for specifying builds and various deployments. If we look
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

In addition, the line defining *DEPLOYMENTS :=* should be modified to add GpsApp like the following:

```
DEPLOYMENTS := Ref GpsApp acdev
```

Given that we have already defined our *Topology* folder in the *GpsApp/Top* line above, we will need to add a placeholder for the Topology of our
GpsApp, which we will fill later. Make the folder *fprime/GpsApp/Top* and add an empty file *mod.mk*. We will fill this folder and mod.mk file
later. Now our make system should be ready for us to build our new module, and take advantage of the code-generation that is available.

## Setting Up Module Make Files and Code

The first thing to do is to setup the module's make files. Each module has two files: a Makefile and a mod.mk file. The makefile contains the module name and little else. The mod.mk file contains a list of all the files and submodules that makeup the module. We should now create the two file, *Makefile* and *mod.mk* using the following content.

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

The file contents are in the following sections.

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

Here it can be seen that we have two actions to do. First we will need to implement a function called *schedIn_handler* and
the second is to implement a command handler for *Gps_ReportLockStatus_cmdHandler*. Both are port calls that will be made
into our component. Ports are found in the GpsComponentAi.xml, however; only our *schedIn* port is used directly. The 
rest are called as part of telemetry, event logging, or command processing and thus have helpful wrappers (seen below).
schedIn represents a call from a rate group driver. It is called a a set rate, e.g. 10Hz based on the rate group to which it 
is connected. We will use this schedIn call to read the UART interface at a set rate. *Gps_ReportLockStatus_cmdHandler* is a 
handler that implements the function for the command we defined in Command.xml. We also have the following out port calls, 
and events that can be used as part of our code. 

#. log_ACTIVITY_HI_Gps_LockAquired: used to emit the event *Gps_Lock_aquired* as defined in Events.xml
#. log_WARNING_HI_Gps_LockLost: used to emit the event *Gps_LockLost* as defined in Events.xml
#. tlmWrite_Gps_Latitude: used to send down *Latitude* telemetry as defined in Telemetry.xml
#. tlmWrite_Gps_Longitude: used to send down *Longitude* telemetry as defined in Telemetry.xml
#. tlmWrite_Gps_Altitude: used to send down *Altitude* telemetry as defined in Telemetry.xml
#. tlmWrite_Gps_Count: used to send down *Count* telemetry as defined in Telemetry.xml
#. sendCommandResponse: used to respond to a sent command. Call this in the above cmdHandler.

In order to make a GPS processor, we need to take the following steps:

1. Initialize the UART in the first call to schedIn
2. Read UART in schedIn
3. Parse UART data in schedIn
4. Downlink telmetry in schedIn
5. If lock is newly found, downlink a *LockAquired* EVR
6. If lock is newly lost, downlink a *LockLost* EVR
7. Downlink a *LockAquired* EVR in commandHandler, if lock is currently held
8. Downlink a *LockLost* EVR in commandHandler, if lock is not currently held
9. Respond from commandHandler with a sendCommandResponse call

Final, implementation found in the following sections

### GpsComponentImpl.cpp (Final)
```
```
### GpsComponentImpl.hpp (Final)
```
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

First the Reference topology is copied into the *Top* folder of *GpsApp* application and we'll grab the deployment makefile 
too.

```
cp -r fprime/Ref/Top fprime/GpsApp/Top
cp fprime/Ref/Makefile fprimt/GpsApp
```

Then update *fprime/GpsApp/Makefile* to set the following line to our deployment name *GpsApp*.

```
DEPLOYMENT := GpsApp
```










