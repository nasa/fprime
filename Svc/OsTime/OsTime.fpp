module Svc {

    @ A time component using OSAL RawTime abstractions
    passive component OsTime {
        include "../Interfaces/TimeInterface.fppi"

        sync input port setEpoch: OsTimeEpoch
    }
}
