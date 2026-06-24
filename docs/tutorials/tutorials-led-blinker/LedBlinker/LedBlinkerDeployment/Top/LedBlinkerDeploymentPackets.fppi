telemetry packets LedBlinkerDeploymentPackets {

  packet CDH id 1 group 1 {
    CdhCore.cmdDisp.CommandsDispatched

    FileHandling.fileUplink.FilesReceived
    FileHandling.fileUplink.PacketsReceived
    FileHandling.fileDownlink.FilesSent
    FileHandling.fileDownlink.PacketsSent
    FileHandling.fileManager.CommandsExecuted

    LedBlinker.cmdSeq.CS_LoadCommands
    LedBlinker.cmdSeq.CS_CancelCommands
    LedBlinker.cmdSeq.CS_CommandsExecuted
    LedBlinker.cmdSeq.CS_SequencesCompleted
    ComCcsds.comQueue.comQueueDepth
    ComCcsds.comQueue.buffQueueDepth
    ComCcsds.commsBufferManager.TotalBuffs
    ComCcsds.commsBufferManager.CurrBuffs
    ComCcsds.commsBufferManager.HiBuffs
    #ComCcsds.tlmSend.SendLevel

    # LED Blinker telemetry channels
    LedBlinker.led.BlinkingState
    LedBlinker.led.LedTransitions

    LedBlinker.rateGroup1.RgMaxTime
    LedBlinker.rateGroup2.RgMaxTime
    LedBlinker.rateGroup3.RgMaxTime
  }

  packet CDHErrors id 2 group 1 {
    CdhCore.$health.PingLateWarnings

    FileHandling.fileUplink.Warnings
    FileHandling.fileDownlink.Warnings
    FileHandling.fileManager.Errors

    LedBlinker.cmdSeq.CS_Errors
    ComCcsds.commsBufferManager.NoBuffs
    ComCcsds.commsBufferManager.EmptyBuffs

    LedBlinker.rateGroup1.RgCycleSlips
    LedBlinker.rateGroup2.RgCycleSlips
    LedBlinker.rateGroup3.RgCycleSlips
  }

  packet SystemRes1 id 5 group 2 {
    LedBlinker.systemResources.MEMORY_TOTAL
    LedBlinker.systemResources.MEMORY_USED
    LedBlinker.systemResources.NON_VOLATILE_TOTAL
    LedBlinker.systemResources.NON_VOLATILE_FREE
  }

  packet SystemRes3 id 6 group 2 {
    LedBlinker.systemResources.CPU
    LedBlinker.systemResources.CPU_00
    LedBlinker.systemResources.CPU_01
    LedBlinker.systemResources.CPU_02
    LedBlinker.systemResources.CPU_03
    LedBlinker.systemResources.CPU_04
    LedBlinker.systemResources.CPU_05
    LedBlinker.systemResources.CPU_06
    LedBlinker.systemResources.CPU_07
    LedBlinker.systemResources.CPU_08
    LedBlinker.systemResources.CPU_09
    LedBlinker.systemResources.CPU_10
    LedBlinker.systemResources.CPU_11
    LedBlinker.systemResources.CPU_12
    LedBlinker.systemResources.CPU_13
    LedBlinker.systemResources.CPU_14
    LedBlinker.systemResources.CPU_15
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
