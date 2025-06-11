module CDHCoreConfig {
    #Base ID for the CDHCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x7000
    
    module QueueSizes {
        constant cmdDisp     = 10
        constant events      = 10
        constant tlmSend     = 10
        constant $health     = 25
    }
    

    module StackSizes {
        constant cmdDisp     = 64 * 1024
        constant events      = 10
        constant tlmSend     = 64 * 1024
    }

    module Priorities {
        constant cmdDisp     = 101
        constant $health     = 100
        constant events      = 99
        constant tlmSend     = 98

    }
}


module CDHCore {
    instance tlmSend: Svc.TlmPacketizer base id CDHCoreConfig.BASE_ID + 0x0300 \
        queue size CDHCoreConfig.QueueSizes.tlmSend \
        stack size CDHCoreConfig.StackSizes.tlmSend \
        priority CDHCoreConfig.Priorities.tlmSend \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        CDHCore::tlmSend.setPacketList(
            Ref::Ref_RefPacketsTlmPackets::packetList, 
            Ref::Ref_RefPacketsTlmPackets::omittedChannels, 
            1
        );
        """
    }

    #instance tlmSend: Svc.TlmChan base id CDHCoreConfig.BASE_ID + 0x0300 \
    #    queue size CDHCoreConfig.QueueSizes.tlmSend \
    #    stack size CDHCoreConfig.StackSizes.tlmSend \
    #    priority CDHCoreConfig.Priorities.tlmSend \
    
}