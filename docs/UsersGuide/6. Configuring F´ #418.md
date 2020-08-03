**Preliminary Outline**

6\. Configuring F´

> i. Configuration Handles for F´
> 
> a. Basic description of configuration handles
> 
> b. How to handle configuration in Python

**  
**

**6. Configuring F´**

**i. Configuration Handles for F´**

Some configurations may be changed during compilation time. The F′
framework has a number of optional features that can be enabled or
disabled by editing a configuration file. Users can change or override
defined *C* macro values that activate or disable code by using complier
flags for different deployment settings. During flight software (FSW)
execution, disabling unnecessary features saves memory and CPU cycles.

**a. Basic description of configuration handles**

The Fw/Cfg directory contains a header file Config.hpp that is used to
configure various properties of the architecture. The developer modifies
the file to adjust the architecture for the requirements of a particular
deployment environment.

1.  
2.  
3.  
4.  
To enable various facilities, set the below to 0 or 1. If set in
compiler flags, defaults are overridden. The F′ architecture supports:
64 bit integers, 32 bit integers, 16 bit integers, and 64 bit floating
point numbers. Not every platform supports these types, so they can be
turned on and off to match the native types of the hardware.

**Types:**

  - FW\_HAS\_64\_BIT = 1

  - FW\_HAS\_32\_BIT = 1

  - FW\_HAS\_16\_BIT = 1

  - FW\_HAS\_64\_BIT = 1

**Boolean values encoded during serialization:** **The value used to
represent true and false may be set in order to choose what a serialized
Boolean looks like.**

  - FW\_SERIALIZE\_TRUE\_VALUE = (0xFF) (Value encoded for boolean
    true.)

  - FW\_SERIALIZE\_FALSE\_VALUE = (0x00) (Value encoded for boolean
    false.)

**Typedefs for various serialization items:** **Special types used by F´
can be set to the native type needed to represent these values. This
allows projects to change the type they used and optimize transmitted
bytes.** Any changes must match the GDS in order to decode the values
correctly. Thus the user may need to update the Gds tyoes as well.

  - FwPacketDescriptorType U32

  - FwOpcodeType U32

  - FwChanIdType U32

  - FwEventIdType U32

  - FwPrmIdType U32

**Defines how big the size of a buffer (or string) type representation
for storing:**

  - FwBuffSizeType U16

**Defines how many bits are used to store an enumeration value defined
in XML during serialization:**

  - FwEnumStoreType I32

**For object facilities:**

  - FW\_OBJECT\_NAMES = 1 (Indicates whether or not object names are
    stored. More memory can be used for tracking objects.)

**Methods to query an object about its name:** (This can be overridden
by derived classes. However, for FW\_OBJECT\_TO\_STRING to work,
FW\_OBJECT\_NAMES must be enabled.)

  - FW\_OBJECT\_TO\_STRING = 1 (Indicates whether or not generated
    objects have toString() methods to dump internals. Requires more
    code.)

  - FW\_OBJECT\_TO\_STRING = 0

**Adds the ability for all component related objects to register
centrally:**

  - FW\_OBJECT\_REGISTRATION = 1 (Indicates whether or not objects can
    register themselves. More code, more object tracking.)

  - FW\_QUEUE\_REGISTRATION = 1 (Indicates whether or not queue
    registration is used.)

**Enable an OS-free schedule:**

  - FW\_BAREMETAL\_SCHEDULER = 1 (Indicates whether or not a baremetal
    scheduler should be used. Alternatively the 0s scheduler is used.)

**Port facilities that allow tracing calls through ports for
debugging:**

  - FW\_PORT\_TRACING = 1 (Indicates whether port calls are traced. More
    code, more visibility into execution.)

**Generates code to connect to serialized ports:**

  - FW\_PORT\_SERIALIZATION = 1 Indicates whether there is code in ports
    to serialize the call. More code, but ability to serialize calls for
    multi-note systems.)

  - 
**Number of bytes to use for serialization IDs:** (More bytes equals
more storage, but the greater number of IDs.)

  - FW\_SERIALIZATION\_TYPE\_ID\_BYTES = 4 (Number of bytes used to
    represent type id. More bytes, more ids.)

**Turn asserts on or off:**

  - FW\_NO\_ASSERT = 1 (Asserts turned off)

  - FW\_FILEID\_ASSERT = 2 (File ID used requires
    DASSERT\_FILE\_ID=\<somevalue\> to be set on the compile command
    line.)

  - FW\_FILENAME\_ASSERT = 3 (Uses the file name in the assert. Image
    stores filenames.)

  - FW\_ASSERT\_LEVEL (FW\_FILENAME\_ASSERT defines the type of assert
    used.)

**Defines maximum length of assert string:**

  - FW\_ASSERT\_TEXT\_SIZE = 120 (Size of string used to store assert
    description.)

NOTE: The user will need to adjust various configuration parameters in
the architecture since some of the above enables may disable the values.

**Indicates the size of the object name stored in the object base
class:** (Larger names will be truncated.)

  - FW\_OBJ\_NAME\_MAX\_SIZE = 80 (Size of object name, if object names
    is enabled. AC limits to 80, truncation occurs above 80.)

**Specifies the size of the buffer to store the description:** (When
querying an object as to an object-specific description.)

  - FW\_OBJ\_TO\_STRING\_BUFFER\_SIZE = 255 (Size of string storing
    toString() text.)

**Specifies how many objects the registry will store:** (For the simple
object registry provided with the framework.)

  - FW\_OBJ\_SIMPLE\_REG\_ENTRIES = 500 (Specifies number of objects
    stored in a simple object registry.)

**Specifies the size of the buffer used to store object names:** (When
dumping the contents of the registry.) Should be \>=
FW\_OBJ\_NAME\_MAX\_SIZE.

  - FW\_OBJ\_SIMPLE\_REG\_BUFF\_SIZE = 255 (Specifies size of object
    registry dump string.)

**Specifies how many queues the registry will store:** (For the simple
queue registry FW\_QUEUE\_REGISTRATION provided with the framework.)

  - FW\_OBJ\_SIMPLE\_REG\_ENTRIES = 100 (Specifies number of queues
    stored in the simple queue registry.)

**Specifies the size of the string holding the queue name for queues:**

  - FW\_QUEUE\_NAME\_MAX\_SIZE = 80 (Maximum size of message queue
    name.)

**Specifies the size of the string holding the task name for active
components and tasks:**

  - FW\_TASK\_NAME\_MAX\_SIZE = 80 (Maximum size of task name.)

**Specifies the size of the buffer that contains a communications
packet:**

  - FW\_COM\_BUFFER\_MAX\_SIZE = 128 (Maximum size of Fw::Com buffer.)

**Specifies the size of the buffer that contains the serialized command
arguments:**

  - FW\_CMD\_ARG\_BUFFER\_MAX\_SIZE
    (FW\_COM\_BUFFER\_MAX\_SIZE—sizeof(FwOpcodeType)—sizeof(FwPacketDescriptorType))

**Specifies the maximum size of a string in a command argument:**

  - FW\_CMD\_STRING\_MAX\_SIZE = 40 (Maximum character size of command
    string arguments.)

NOTE: Normally when a command is deserialized, the handler checks to see
if there are any leftover bytes in the buffer. If there are, it assumes
that the command was corrupted somehow since the serialized size should
match the serialized size of the argument list. In some cases, command
buffers are padded so the data can be larger than the serialized size of
the command.

Setting the below to zero will disable the check to the detriment of not
detecting commands that are too large.

  - FW\_CMD\_CHECK\_RESIDUAL = 1 (Check for leftover command bytes.)

**Specifies the size of the buffer that contains the serialized log
arguments:**

  - FW\_LOG\_BUFFER\_MAX\_SIZE
    (FW\_COM\_BUFFER\_MAX\_SIZE—sizeof(FwEventIdType)
    —sizeof(FwPacketDescriptorType))

**Specifies the maximum size of a string in a log event:**

  - FW\_LOG\_STRING\_MAX\_SIZE = 100 (Maximum size of log string
    parameter type.)

**Specifies the size of the buffer that contains the serialized
telemetry value:**

  - FW\_TLM\_BUFFER\_MAX\_SIZE
    (FW\_COM\_BUFFER\_MAX\_SIZE—sizeof(FwChanIdType)—sizeof(FwPacketDescriptorType))

**Specifies the maximum size of a string in a telemetry channel:**

  - FW\_TLM\_STRING\_MAX\_SIZE = 40 (Maximum size of channelized
    telemetry string type.)

**Specifies the size of the buffer that contains the serialized
parameter value:**

  - FW\_PARAM\_BUFFER\_MAX\_SIZE
    (FW\_COM\_BUFFER\_MAX\_SIZE—sizeof(FwPrmIdType)—sizeof(FwPacketDescriptorType))

**Specifies the maximum size of a string in a parameter:**

  - FW\_PARAM\_STRING\_MAX\_SIZE = 40 (Maximum size of parameter string
    type.)

**Specifies the maximum size of a file upload chunk:**

  - FW\_FILE\_BUFFER\_MAX\_SIZE = 255 (Maximum size of file buffer
    (i.e., chunk of file).

**Specifies the maximum size of a string in an interface call:**

  - FW\_INTERNAL\_INTERFACE\_STRING\_MAX\_SIZE = 40 (Maximum size of
    interface string parameter type.)

**Enables text logging of events as well as data logging:** (Adds a
second logging port for text output.)

  - FW\_ENABLE\_TEXT\_LOGGING = 1 (Indicates whether text logging is
    turned on.)

**Defines the size of the text log string buffer:** (Should be large
enough for format string and arguments.)

  - FW\_LOG\_TEXT\_BUFFER\_SIZE = 256 (Maximum size of string for text
    log message.)

**Defines if serializables have** toString() **method:** (Turning this
off saves code space and string constants; however, it must be enabled
if text logging is enabled.)

  - FW\_SERIALIZABLE\_TO\_STRING = 1 (Indicates if autocoded
    serializables have toString() methods.)

  - FW\_SERIALIZABLE\_TO\_STRING\_BUFFER\_SIZE = 255 (Size of string to
    store toString() string output.)

**Setting to enable AMPCS compatibility:** (This breaks regular ISF GUI
compatibility.)

  - FW\_AMPCS\_COMPATIBLE = 0 (Whether or not JPL AMPCS ground system
    support is enabled.)

**Defines enumeration for Time base types:**

  - TB\_NONE (No time base has been established.)

  - TB\_PROC\_TIME (Indicates time is processor cycle time. This is not
    tied to external time.)

  - TB\_WORKSTATION\_TIME (Time as reported on workstation where
    software is running. Used for testing.)

  - TB\_DONT\_CARE = 0xFFFF (Do not care value for sequences. If
    FwTimeBaseStoreType is changed, this value should be changed.)

**Specifies how many bits are used to store the time base:**

  - FwTimeBaseStoreType U16 (Storage conversion for time base in
    scripts/ground interface.)

  - FwTimeContextStoreType U8 (Storage conversion for time context in
    scripts/ground interface.)

  - FW\_CONTEXT\_DONT\_CARE 0xFF (Do not care value for time contexts in
    sequences.)

**Settings configure whether or not the timebase and context values for
the** Fw::Time **class are used:** (Some systems may not use or need
these fields.)

  - FW\_USE\_TIME\_BASE = 1 (Whether or not to use the time base.)

  - FW\_USE\_TIME\_CONTEXT = 1 (Whether or not to serialize the time
    context.)

NOTE: configuration checks are in Fw/Cfg/ConfigCheck.cpp in order to
have the type definitions in Fw/Types/BasicTypes available.

The file AcConstants.ini contains a set of values for variables used in
the code generator. The file follows the Python ConfigParser based on
.ini files. Using the .ini file allows component features such as
opcodes and port numbers to be changed without modifying the XML
component.

**b. How to handle configuration in Python**

The F′ ground support equipment (GSE) is implemented in Python, a
programming language acting as a configuration file that can be easily
customized, and depends on a few Python packages. It requires little
effort to install. The only configuration required for Python Code is
setting the environment variable that tells the GSE where to find the
generated Python dictionaries for the target application.

**Defines number of ports:** (Syntax in the file follows the Python
ConfigParser .ini file specification.)

  - ActiveRateGroupOutputPorts = 10 (Number of rate group member output
    ports for ActiveRateGroup.)

  - CmdDispatcherComponentCommandPorts = 30 (Used for command and
    registration ports.)

  - CmdDispatcherSequencePorts = 5 (Used for uplink/sequencer
    buffer/response ports.)

  - RateGroupDriverRateGroupPorts = 3 (Used to drive rate groups.)

  - HealthPingPorts = 25 (Used to ping active components.)
