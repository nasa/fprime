# ======================================================================
# \title Os/Models/RawTime.fpp
# \brief FPP type definitions for Os/RawTime.hpp concepts
# ======================================================================

module Os {

    @ FPP shadow-enum representing Os::RawTime::Status
    enum RawTimeStatus {
        OP_OK,          @<  Operation was successful
        OP_OVERFLOW,    @< Operation result caused an overflow
        INVALID_PARAMS, @< Parameters invalid for current platform
        OTHER_ERROR,    @< All other errors
    }

}
