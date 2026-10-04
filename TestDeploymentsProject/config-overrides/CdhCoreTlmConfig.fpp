# TestDeploymentsProject override of Svc/Subtopologies/CdhCore/CdhCoreConfig/CdhCoreTlmConfig.fpp
#
# Selects Svc.TlmPacketizer (instead of the framework default Svc.TlmChan) as the CdhCore telemetry
# instance so that the Ref deployment exercises packetized telemetry and its data product recording.
module CdhCore{

    instance tlmSend: Svc.TlmPacketizer base id CdhCoreConfig.BASE_ID + 0x06000 \
        queue size CdhCoreConfig.QueueSizes.tlmSend \
        stack size CdhCoreConfig.StackSizes.tlmSend \
        priority CdhCoreConfig.Priorities.tlmSend \
        cpu CdhCoreConfig.CpuAffinities.tlmSend \
    {
        # NOTE: The name Ref is specific to the Reference deployment, Ref
        phase Fpp.ToCpp.Phases.configComponents """
        CdhCore::tlmSend.setPacketList(
            Ref::Ref_RefPacketsTlmPackets::packetList,
            Ref::Ref_RefPacketsTlmPackets::omittedChannels,
            1
        );
        """
    }
}
