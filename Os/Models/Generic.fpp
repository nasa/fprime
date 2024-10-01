# ======================================================================
# \title Os/Models/Generic.fpp
# \brief FPP type definitions for Os/Os.hpp concepts
# ======================================================================

module Os {
module Generic {
@ FPP shadow-enum representing Os::Generic::Status
enum Status {
    OP_OK, @< operation okay
    ERROR, @< error return value
}
}
}

