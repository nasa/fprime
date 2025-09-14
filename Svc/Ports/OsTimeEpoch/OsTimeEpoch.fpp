#####
# OsTimeEpoch:
#
# Used to set Epoch time pairs for OsTime
#####


module Svc {

    @ A pair of timestamps representing an Epoch time in
    @ an Fw::Time and Os::RawTime object
    port OsTimeEpoch(
        fw_time: Fw.Time,
        os_time: Os.RawTime
    )

}
