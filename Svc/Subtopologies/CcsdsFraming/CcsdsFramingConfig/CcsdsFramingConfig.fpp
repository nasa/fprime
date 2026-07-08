module CcsdsFramingConfig {
    #Base ID for the CcsdsFraming Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x07000000

    # Buffer management constants
    module BuffMgr {
        constant frameAccumulatorSize = 2048
    }
}
