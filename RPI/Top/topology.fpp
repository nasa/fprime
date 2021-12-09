module RPI {

  topology RPI {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance $health
    instance chanTlm
    instance cmdDisp
    instance cmdSeq
    instance comm
    instance downlink
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance fileDownlink
    instance fileUplink
    instance fileUplinkBufferManager
    instance gpio17Drv
    instance gpio23Drv
    instance gpio24Drv
    instance gpio25Drv
    instance ledDrv
    instance linuxTime
    instance linuxTimer
    instance prmDb
    instance rateGroup10HzComp
    instance rateGroup1HzComp
    instance rateGroupDriverComp
    instance rpiDemo
    instance spiDrv
    instance staticMemory
    instance textLogger
    instance uartDrv
    instance uplink

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    health connections instance $health

    telemetry connections instance chanTlm

    text event connections instance textLogger

    time connections instance linuxTime

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Downlink {
      chanTlm.PktSend -> downlink.comIn
      downlink.bufferDeallocate -> fileDownlink.bufferReturn
      downlink.framedOut -> comm.send
      eventLogger.PktSend -> downlink.comIn
      fileDownlink.bufferSendOut -> downlink.bufferIn
    }

    connections RateGroups {

      # Timer
      linuxTimer.CycleOut -> rateGroupDriverComp.CycleIn

      # 10 Hz rate group
      rateGroupDriverComp.CycleOut[0] -> rateGroup10HzComp.CycleIn
      rateGroup10HzComp.RateGroupMemberOut[0] -> rpiDemo.Run
      rateGroup10HzComp.RateGroupMemberOut[1] -> fileDownlink.Run

      # 1 Hz rate group
      rateGroupDriverComp.CycleOut[1] -> rateGroup1HzComp.CycleIn
      rateGroup1HzComp.RateGroupMemberOut[0] -> chanTlm.Run
      rateGroup1HzComp.RateGroupMemberOut[1] -> $health.Run
      rateGroup1HzComp.RateGroupMemberOut[2] -> rpiDemo.Run
      rateGroup1HzComp.RateGroupMemberOut[3] -> cmdSeq.schedIn

    }

    connections StaticMemory {
      comm.allocate -> staticMemory.bufferAllocate[0]
      comm.deallocate -> staticMemory.bufferDeallocate[1]
      downlink.framedAllocate -> staticMemory.bufferAllocate[1]
      uplink.framedDeallocate -> staticMemory.bufferDeallocate[0]
    }

    connections XML {
      cmdDisp.seqCmdStatus -> uplink.cmdResponseIn
      cmdDisp.seqCmdStatus[1] -> cmdSeq.cmdResponseIn[0]
      cmdSeq.comCmdOut[0] -> cmdDisp.seqCmdBuff[1]
      comm.$recv[0] -> uplink.framedIn[0]
      eventLogger.FatalAnnounce[0] -> fatalHandler.FatalReceive[0]
      fileUplink.bufferSendOut[0] -> fileUplinkBufferManager.bufferSendIn[0]
      rpiDemo.GpioRead[0] -> gpio25Drv.gpioRead[0]
      rpiDemo.GpioRead[1] -> gpio17Drv.gpioRead[0]
      rpiDemo.GpioWrite[0] -> gpio23Drv.gpioWrite[0]
      rpiDemo.GpioWrite[1] -> gpio24Drv.gpioWrite[0]
      rpiDemo.GpioWrite[2] -> ledDrv.gpioWrite[0]
      rpiDemo.SpiReadWrite[0] -> spiDrv.SpiReadWrite[0]
      rpiDemo.UartBuffers[0] -> uartDrv.readBufferSend[0]
      rpiDemo.UartWrite[0] -> uartDrv.serialSend[0]
      rpiDemo.prmGetOut[0] -> prmDb.getPrm[0]
      rpiDemo.prmSetOut[0] -> prmDb.setPrm[0]
      uartDrv.serialRecv[0] -> rpiDemo.UartRead[0]
      uplink.bufferAllocate[0] -> fileUplinkBufferManager.bufferGetCallee[0]
      uplink.bufferDeallocate[0] -> fileUplinkBufferManager.bufferSendIn[0]
      uplink.bufferOut[0] -> fileUplink.bufferSendIn[0]
      uplink.comOut[0] -> cmdDisp.seqCmdBuff[0]
    }

  }

}
