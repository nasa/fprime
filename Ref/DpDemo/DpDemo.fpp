module Ref {
    @ DP Demo
    active component DpDemo {

        enum ColorEnum {
            RED
            GREEN
            BLUE
        }

        enum DpReqType {
            IMMEDIATE
            ASYNC
        }

        type StringAlias = string
        type BoolAlias = bool
        type I32Alias = I32
        type F64Alias = F64

        @ Array of floats
        array F32Array = [3] F32

        @ Array of integers
        array U32Array = [5] U32

        @ Array of strings
        array StringArray = [2] string

        @ Array of array of strings
        array ArrayOfStringArray = [3] StringArray

        @ Array of booleans
        array BooleanArray = [2] bool

        @ Array of enumerations
        array EnumArray = [3] ColorEnum

        array ArrayOfStructs = [3] StructWithStringMembers

        struct ColorInfoStruct {
            Color: ColorEnum
        }

        struct StructWithStringMembers {
            stringMember: string,
            stringArrayMember: StringArray
        }

        struct StructWithEverything {
            integerMember: I32Alias,
            floatMember: F32,
            stringMember: string,
            booleanMember: bool,
            enumMember: ColorEnum,
            arrayMemberU32: [2] U32Array,
            F32Array: F32Array,
            U32Array: U32Array,
            enumArray: EnumArray
            stringArray: StringArray,
            booleanArray: BooleanArray,
            structWithStrings: StructWithStringMembers
            nestedArrays: ArrayOfStringArray
        }

        @ Select color
        async command SelectColor(color: ColorEnum) opcode 0

        @ Command for generating a DP
        sync command Dp(reqType: DpReqType, $priority: U32)
        
        @ Color selected event
        event ColorSelected(color: ColorEnum) severity activity high id 0 format "Color selected {}"

        @ DP started event
        event DpStarted(records: U32) \
            severity activity low \
            id 1 \
            format "Writing {} DP records"

        @ DP complete event
        event DpComplete(records: U32) \
            severity activity low \
            id 2 \
            format "Finished writing {} DP records"

        event DpRecordFull(records: U32, bytes: U32) \
            severity warning low \
            id 3 \
            format "DP container full with {} records and {} bytes. Closing DP."

        event DpMemRequested($size: FwSizeType) \
            severity activity low \
            id 4 \
            format "Requesting {} bytes for DP"

        event DpMemReceived($size: FwSizeType) \
            severity activity low \
            id 5 \
            format "Received {} bytes for DP"

        event DpMemoryFail \
            severity warning high \
            id 6 \
            format "Failed to acquire a DP buffer"

        event DpsNotConnected \
            severity warning high \
            id 7 \
            format "DP Ports not connected!"

        @ Example port: receiving calls from the rate group
        sync input port run: Svc.Sched

        # @ Example parameter
        # param PARAMETER_NAME: U32

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @ Port to set the value of a parameter
        param set port prmSetOut

        @ Data product get port
        product get port productGetOut

        @ Data product request port
        product request port productRequestOut

        @ Data product receive port
        async product recv port productRecvIn

        @ Data product send port
        product send port productSendOut

        @ Data product record - struct record
        product record ColorInfoStructRecord: ColorInfoStruct id 0

        @ Data product record - enum
        product record ColorEnumRecord: ColorEnum id 1

        @ Data product record - string
        product record StringRecord: StringAlias id 2

        @ Data product record - boolean
        product record BooleanRecord: BoolAlias id 3

        @ Data product record - I32
        product record I32Record: I32Alias id 4

        @ Data product record - F64
        product record F64Record: F64Alias id 5

        @ Data product record - U32 array record
        product record U32ArrayRecord: U32Array id 6

        @ Data product record - F32 array record
        product record F32ArrayRecord: F32Array id 7

        @ Data product record - boolean array record
        product record BooleanArrayRecord: BooleanArray id 8

        @ Data product record - enum array record
        product record EnumArrayRecord: EnumArray id 9

        @ Data product record - string array record
        product record StringArrayRecord: string array id 10

        @ Data product record - array record (structs)
        product record StructArrayRecord: StructWithStringMembers array id 11

        @ Data product record - array record (arrays)
        product record ArrayArrayRecord: StringArray array id 12

        @ Data product record - array record (string arrays)
        product record ArrayOfStringArrayRecord: ArrayOfStringArray id 13

        @ Data product record - struct record
        product record StructWithEverythingRecord: StructWithEverything id 14

        @ Data product record - array of structs
        product record ArrayOfStructsRecord: ArrayOfStructs id 15

        @ Data product container
        product container DpDemoContainer id 0 default priority 10

    }
}
