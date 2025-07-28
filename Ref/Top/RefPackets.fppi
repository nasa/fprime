telemetry packets RefPackets {

  packet CDH id 1 group 1 {
    CdhCore.cmdDisp.CommandsDispatched

    FileHandling.fileUplink.FilesReceived
    FileHandling.fileUplink.PacketsReceived
    FileHandling.fileDownlink.FilesSent
    FileHandling.fileDownlink.PacketsSent
    FileHandling.fileManager.CommandsExecuted

    Ref.cmdSeq.CS_LoadCommands
    Ref.cmdSeq.CS_CancelCommands
    Ref.cmdSeq.CS_CommandsExecuted
    Ref.cmdSeq.CS_SequencesCompleted
    ComCcsds.comQueue.comQueueDepth
    ComCcsds.comQueue.buffQueueDepth
    ComCcsds.commsBufferManager.TotalBuffs
    ComCcsds.commsBufferManager.CurrBuffs
    ComCcsds.commsBufferManager.HiBuffs
    #ComCcsds.tlmSend.SendLevel

    Ref.rateGroup1Comp.RgMaxTime
    Ref.rateGroup2Comp.RgMaxTime
    Ref.rateGroup3Comp.RgMaxTime
  }

  packet CDHErrors id 2 group 1 {
    CdhCore.$health.PingLateWarnings

    FileHandling.fileUplink.Warnings
    FileHandling.fileDownlink.Warnings
    FileHandling.fileManager.Errors

    Ref.cmdSeq.CS_Errors
    ComCcsds.commsBufferManager.NoBuffs
    ComCcsds.commsBufferManager.EmptyBuffs

    Ref.rateGroup1Comp.RgCycleSlips
    Ref.rateGroup2Comp.RgCycleSlips
    Ref.rateGroup3Comp.RgCycleSlips
  }

  packet DriveTlm id 3 group 1 {
    Ref.pingRcvr.PR_NumPings
    Ref.sendBuffComp.PacketsSent
    Ref.sendBuffComp.NumErrorsInjected
    Ref.sendBuffComp.Parameter3
    Ref.sendBuffComp.Parameter4
    Ref.sendBuffComp.SendState
    Ref.recvBuffComp.PktState
    Ref.recvBuffComp.Sensor1
    Ref.recvBuffComp.Sensor2
    Ref.recvBuffComp.Parameter1
    Ref.recvBuffComp.Parameter2
    Ref.blockDrv.BD_Cycles
  }

  packet SigGenSum id 4 group 1 {
    Ref.SG1.Output
    Ref.SG1.Type
    Ref.SG2.Output
    Ref.SG2.Type
    Ref.SG3.Output
    Ref.SG3.Type
    Ref.SG4.Output
    Ref.SG4.Type
    Ref.SG5.Output
    Ref.SG5.Type
  }

  packet SystemRes1 id 5 group 2 {
    Ref.systemResources.MEMORY_TOTAL
    Ref.systemResources.MEMORY_USED
    Ref.systemResources.NON_VOLATILE_TOTAL
    Ref.systemResources.NON_VOLATILE_FREE
  }

  packet SystemRes3 id 6 group 2 {
    Ref.systemResources.CPU
    Ref.systemResources.CPU_00
    Ref.systemResources.CPU_01
    Ref.systemResources.CPU_02
    Ref.systemResources.CPU_03
    Ref.systemResources.CPU_04
    Ref.systemResources.CPU_05
    Ref.systemResources.CPU_06
    Ref.systemResources.CPU_07
    Ref.systemResources.CPU_08
    Ref.systemResources.CPU_09
    Ref.systemResources.CPU_10
    Ref.systemResources.CPU_11
    Ref.systemResources.CPU_12
    Ref.systemResources.CPU_13
    Ref.systemResources.CPU_14
    Ref.systemResources.CPU_15
  }

  packet SigGen1Info id 10 group 2 {
    Ref.SG1.Info
  }

  packet SigGen2Info id 11 group 2 {
    Ref.SG2.Info
  }

  packet SigGen3Info id 12 group 2 {
    Ref.SG3.Info
  }

  packet SigGen4Info id 13 group 2 {
    Ref.SG4.Info
  }

  packet SigGen5Info id 14 group 2 {
    Ref.SG5.Info
  }

  packet SigGen1 id 15 group 3 {
    Ref.SG1.PairOutput
    Ref.SG1.History
    Ref.SG1.PairHistory
    Ref.SG1.DpBytes
    Ref.SG1.DpRecords
  }

  packet SigGen2 id 16 group 3 {
    Ref.SG2.PairOutput
    Ref.SG2.History
    Ref.SG2.PairHistory
    Ref.SG2.DpBytes
    Ref.SG2.DpRecords
  }

  packet SigGen3 id 17 group 3 {
    Ref.SG3.PairOutput
    Ref.SG3.History
    Ref.SG3.PairHistory
    Ref.SG3.DpBytes
    Ref.SG3.DpRecords
  }

  packet SigGen4 id 18 group 3 {
    Ref.SG4.PairOutput
    Ref.SG4.History
    Ref.SG4.PairHistory
    Ref.SG4.DpBytes
    Ref.SG4.DpRecords
  }

  packet SigGen5 id 19 group 3 {
    Ref.SG5.PairOutput
    Ref.SG5.History
    Ref.SG5.PairHistory
    Ref.SG5.DpBytes
    Ref.SG5.DpRecords
  }

  packet TypeDemo id 20 group 3 {
    Ref.typeDemo.ChoiceCh
    Ref.typeDemo.ChoicesCh
    Ref.typeDemo.ExtraChoicesCh
    Ref.typeDemo.ChoicePairCh
    Ref.typeDemo.ChoiceSlurryCh
    Ref.typeDemo.Float1Ch
    Ref.typeDemo.Float2Ch
    Ref.typeDemo.Float3Ch
    Ref.typeDemo.FloatSet
    Ref.typeDemo.ScalarStructCh
    Ref.typeDemo.ScalarU8Ch
    Ref.typeDemo.ScalarU16Ch
    Ref.typeDemo.ScalarU32Ch
    Ref.typeDemo.ScalarU64Ch
    Ref.typeDemo.ScalarI8Ch
    Ref.typeDemo.ScalarI16Ch
    Ref.typeDemo.ScalarI32Ch
    Ref.typeDemo.ScalarI64Ch
    Ref.typeDemo.ScalarF32Ch
    Ref.typeDemo.ScalarF64Ch
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
