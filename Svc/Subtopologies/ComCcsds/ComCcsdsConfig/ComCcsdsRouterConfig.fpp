module ComCcsds {

    # NOTE: name 'router' is used for the uplink router instance for better subtopology
    # interoperability. Projects may swap the router implementation by overriding this
    # configuration file with an alternate instance definition. The replacement component
    # must implement the Svc.Router interface (Svc/Interfaces/Router.fpp) ports referenced
    # by the ComCcsds topologies: dataIn, dataReturnOut, commandOut, cmdResponseIn,
    # fileOut, and fileBufferReturnIn.
    instance router: Svc.FprimeRouter base id ComCcsdsConfig.BASE_ID + 0x03000

    # Example: comment the block above and uncomment a block like the following to use a
    # custom router implementation instead of Svc.FprimeRouter
    # instance router: MyModule.MyRouter base id ComCcsdsConfig.BASE_ID + 0x03000
}
