telemetry packets RefPackets {

  packet CDH id 1 group 1 {
    CdhCore.cmdDisp.CommandsDispatched

    FileHandling.fileUplink.FilesReceived
    FileHandling.fileUplink.PacketsReceived
    FileHandling.fileDownlink.FilesSent
    FileHandling.fileDownlink.PacketsSent
    FileHandling.fileManager.CommandsExecuted

    cmdSeq.CS_LoadCommands
    cmdSeq.CS_CancelCommands
    cmdSeq.CS_CommandsExecuted
    cmdSeq.CS_SequencesCompleted
    ComCcsds.comQueue.comQueueDepth
    ComCcsds.comQueue.buffQueueDepth
    ComCcsds.commsBufferManager.TotalBuffs
    ComCcsds.commsBufferManager.CurrBuffs
    ComCcsds.commsBufferManager.HiBuffs
    #CdhCore.tlmSend.GroupConfigs
    #CdhCore.tlmSend.SectionEnabled

    rateGroup1Comp.RgMaxTime
    rateGroup2Comp.RgMaxTime
    rateGroup3Comp.RgMaxTime
  }

  packet CDHErrors id 2 group 1 {
    CdhCore.$health.PingLateWarnings
    CdhCore.cmdDisp.CommandsDropped

    FileHandling.fileUplink.Warnings
    FileHandling.fileDownlink.Warnings
    FileHandling.fileManager.Errors

    cmdSeq.CS_Errors
    ComCcsds.commsBufferManager.NoBuffs
    ComCcsds.commsBufferManager.EmptyBuffs

    rateGroup1Comp.RgCycleSlips
    rateGroup2Comp.RgCycleSlips
    rateGroup3Comp.RgCycleSlips
  }

  packet DriveTlm id 3 group 1 {
    pingRcvr.PR_NumPings
    sendBuffComp.PacketsSent
    sendBuffComp.NumErrorsInjected
    sendBuffComp.Parameter3
    sendBuffComp.Parameter4
    sendBuffComp.SendState
    recvBuffComp.PktState
    recvBuffComp.Sensor1
    recvBuffComp.Sensor2
    recvBuffComp.Parameter1
    recvBuffComp.Parameter2
    blockDrv.BD_Cycles
  }

  packet SigGenSum id 4 group 1 {
    SG1.Output
    SG1.Type
    SG2.Output
    SG2.Type
    SG3.Output
    SG3.Type
    SG4.Output
    SG4.Type
    SG5.Output
    SG5.Type
  }

  packet SystemRes1 id 5 group 2 {
    systemResources.MEMORY_TOTAL
    systemResources.MEMORY_USED
    systemResources.NON_VOLATILE_TOTAL
    systemResources.NON_VOLATILE_FREE
  }

  packet SystemRes3 id 6 group 2 {
    systemResources.CPU
    systemResources.CPU_00
    systemResources.CPU_01
    systemResources.CPU_02
    systemResources.CPU_03
    systemResources.CPU_04
    systemResources.CPU_05
    systemResources.CPU_06
    systemResources.CPU_07
    systemResources.CPU_08
    systemResources.CPU_09
    systemResources.CPU_10
    systemResources.CPU_11
    systemResources.CPU_12
    systemResources.CPU_13
    systemResources.CPU_14
    systemResources.CPU_15
  }

  packet SigGen1Info id 10 group 2 {
    SG1.Info
  }

  packet SigGen2Info id 11 group 2 {
    SG2.Info
  }

  packet SigGen3Info id 12 group 2 {
    SG3.Info
  }

  packet SigGen4Info id 13 group 2 {
    SG4.Info
  }

  packet SigGen5Info id 14 group 2 {
    SG5.Info
  }

  packet SigGen1 id 15 group 3 {
    SG1.PairOutput
    SG1.History
    SG1.PairHistory
    SG1.DpBytes
    SG1.DpRecords
  }

  packet SigGen2 id 16 group 3 {
    SG2.PairOutput
    SG2.History
    SG2.PairHistory
    SG2.DpBytes
    SG2.DpRecords
  }

  packet SigGen3 id 17 group 3 {
    SG3.PairOutput
    SG3.History
    SG3.PairHistory
    SG3.DpBytes
    SG3.DpRecords
  }

  packet SigGen4 id 18 group 3 {
    SG4.PairOutput
    SG4.History
    SG4.PairHistory
    SG4.DpBytes
    SG4.DpRecords
  }

  packet SigGen5 id 19 group 3 {
    SG5.PairOutput
    SG5.History
    SG5.PairHistory
    SG5.DpBytes
    SG5.DpRecords
  }

  packet TypeDemo id 20 group 3 {
    typeDemo.ChoiceCh
    typeDemo.ChoicesCh
    typeDemo.ExtraChoicesCh
    typeDemo.ChoicePairCh
    typeDemo.ChoiceSlurryCh
    typeDemo.Float1Ch
    typeDemo.Float2Ch
    typeDemo.Float3Ch
    typeDemo.FloatSet
    typeDemo.ScalarStructCh
    typeDemo.ScalarU8Ch
    typeDemo.ScalarU16Ch
    typeDemo.ScalarU32Ch
    typeDemo.ScalarU64Ch
    typeDemo.ScalarI8Ch
    typeDemo.ScalarI16Ch
    typeDemo.ScalarI32Ch
    typeDemo.ScalarI64Ch
    typeDemo.ScalarF32Ch
    typeDemo.ScalarF64Ch
  }

  packet DataProducts id 21 group 3 {
    DataProducts.dpCat.CatalogDps
    DataProducts.dpCat.DpsSent

    DataProducts.dpMgr.NumSuccessfulAllocations
    DataProducts.dpMgr.NumFailedAllocations
    DataProducts.dpMgr.NumDataProducts
    DataProducts.dpMgr.NumBytes

    DataProducts.dpWriter.NumBuffersReceived
    DataProducts.dpWriter.NumBytesWritten
    DataProducts.dpWriter.NumSuccessfulWrites
    DataProducts.dpWriter.NumFailedWrites
    DataProducts.dpWriter.NumErrors

    DataProducts.dpBufferManager.TotalBuffs
    DataProducts.dpBufferManager.CurrBuffs
    DataProducts.dpBufferManager.HiBuffs
    DataProducts.dpBufferManager.NoBuffs
    DataProducts.dpBufferManager.EmptyBuffs

    DataProducts.dpZLibCompressorBufferManager.TotalBuffs
    DataProducts.dpZLibCompressorBufferManager.CurrBuffs
    DataProducts.dpZLibCompressorBufferManager.HiBuffs
    DataProducts.dpZLibCompressorBufferManager.NoBuffs
    DataProducts.dpZLibCompressorBufferManager.EmptyBuffs
  }

  packet Version1 id 22 group 2 {
    CdhCore.version.FrameworkVersion
    CdhCore.version.ProjectVersion
  }

  packet Version_Library1 id 23 group 2 {
    CdhCore.version.LibraryVersion01
    CdhCore.version.LibraryVersion02
  }

  packet Version_Library2 id 24 group 2 {
    CdhCore.version.LibraryVersion03
    CdhCore.version.LibraryVersion04
  }

  packet Version_Library3 id 25 group 2 {
    CdhCore.version.LibraryVersion05
    CdhCore.version.LibraryVersion06
  }

  packet Version_Library4 id 26 group 2 {
    CdhCore.version.LibraryVersion07
    CdhCore.version.LibraryVersion08
  }

  packet Version_Library5 id 27 group 2 {
    CdhCore.version.LibraryVersion09
    CdhCore.version.LibraryVersion10
  }

  packet Version_Custom1 id 28 group 2 {
    CdhCore.version.CustomVersion01
  }

  packet Version_Custom2 id 29 group 2 {
    CdhCore.version.CustomVersion02
  }

  packet Version_Custom3 id 30 group 2 {
    CdhCore.version.CustomVersion03
  }

  packet Version_Custom4 id 31 group 2 {
    CdhCore.version.CustomVersion04
  }

  packet Version_Custom5 id 32 group 2 {
    CdhCore.version.CustomVersion05
  }

  packet Version_Custom6 id 33 group 2 {
    CdhCore.version.CustomVersion06
  }

  packet Version_Custom7 id 34 group 2 {
    CdhCore.version.CustomVersion07
  }

  packet Version_Custom8 id 35 group 2 {
    CdhCore.version.CustomVersion08
  }

  packet Version_Custom9 id 36 group 2 {
    CdhCore.version.CustomVersion09
  }

  packet Version_Custom10 id 37 group 2 {
    CdhCore.version.CustomVersion10
  }

} omit {
  CdhCore.cmdDisp.CommandErrors
}
