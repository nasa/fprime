module CDHCoreConfig {
    #Base ID for the CDHCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x3000
    
    module QueueSizes {
        constant cmdDisp     = 10
        constant events      = 10
        constant tlmSend     = 10
        constant $health     = 25
    }
    

    module StackSizes {
        constant cmdDisp     = 64 * 1024
        constant events      = 64 * 1024
        constant tlmSend     = 64 * 1024
    }

    module Priorities {
        constant cmdDisp     = 102
        constant $health     = 101
        constant events      = 100
        constant tlmSend     = 99

    }
}


module CDHCore {

    instance tlmSend: Svc.TlmChan base id CDHCoreConfig.BASE_ID + 0x0700 \
        queue size CDHCoreConfig.QueueSizes.tlmSend \
        stack size CDHCoreConfig.StackSizes.tlmSend \
        priority CDHCoreConfig.Priorities.tlmSend \

    # Uncomment the following block and comment the above block to use TlmPacketizer instead of TlmChan
    #instance tlmSend: Svc.TlmPacketizer base id CDHCoreConfig.BASE_ID + 0x0700 \
    #    queue size CDHCoreConfig.QueueSizes.tlmSend \
    #    stack size CDHCoreConfig.StackSizes.tlmSend \
    #    priority CDHCoreConfig.Priorities.tlmSend \
    #{
    #    # NOTE: The Name Ref is specific to the Reference deployment, Ref
    #    # This name will need to be updated if wishing to use this in a custom deployment
    #    phase Fpp.ToCpp.Phases.configComponents """
    #    CDHCore::tlmSend.setPacketList(
    #        Ref::Ref_RefPacketsTlmPackets::packetList, 
    #        Ref::Ref_RefPacketsTlmPackets::omittedChannels, 
    #        1
    #    );
    #    """
    #}


}