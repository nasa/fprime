# ======================================================================
# \title Os/Models/Generic.fpp
# \brief FPP type definitions for Os/Os.hpp concepts
# ======================================================================

module Os {
@ FPP shadow-enum representing Os::Generic::Status
enum GenericStatus {
    OP_OK, @< operation okay
    ERROR, @< error return value
}
}

