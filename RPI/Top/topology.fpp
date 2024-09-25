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
    instance deframer
    instance downlink
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance fileDownlink
    instance fileUplink
    instance frameAccumulator
    instance commsBufferManager
    instance gpio17Drv
    instance gpio23Drv
    instance gpio24Drv
    instance gpio25Drv
    instance ledDrv
    instance posixTime
    instance linuxTimer
    instance prmDb
    instance rateGroup10HzComp
    instance rateGroup1HzComp
    instance rateGroupDriverComp
    instance rpiDemo
    instance spiDrv
    instance textLogger
    instance uartDrv
    instance uartBufferManager
    instance uplinkRouter

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    health connections instance $health

    param connections instance prmDb

    telemetry connections instance chanTlm

    text event connections instance textLogger

    time connections instance posixTime

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Downlink {
      chanTlm.PktSend -> downlink.comIn
      downlink.bufferDeallocate -> fileDownlink.bufferReturn
      downlink.framedOut -> comm.$send
      eventLogger.PktSend -> downlink.comIn
      fileDownlink.bufferSendOut -> downlink.bufferIn
    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections GPIO {
      rpiDemo.GpioRead -> gpio25Drv.gpioRead
      rpiDemo.GpioRead -> gpio17Drv.gpioRead
      rpiDemo.GpioWrite[0] -> gpio23Drv.gpioWrite
      rpiDemo.GpioWrite[1] -> gpio24Drv.gpioWrite
      rpiDemo.GpioWrite[2] -> ledDrv.gpioWrite
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

    connections Sequencer {
      cmdDisp.seqCmdStatus -> cmdSeq.cmdResponseIn
      cmdSeq.comCmdOut -> cmdDisp.seqCmdBuff
    }

    connections SPI {
      rpiDemo.SpiReadWrite -> spiDrv.SpiReadWrite
    }

    connections MemoryMgmt {
      comm.allocate -> commsBufferManager.bufferGetCallee
      comm.deallocate -> commsBufferManager.bufferSendIn
      downlink.framedAllocate -> commsBufferManager.bufferGetCallee
      fileUplink.bufferSendOut -> commsBufferManager.bufferSendIn
      frameAccumulator.frameAllocate -> commsBufferManager.bufferGetCallee
      frameAccumulator.dataDeallocate -> commsBufferManager.bufferSendIn
      uplinkRouter.bufferDeallocate -> commsBufferManager.bufferSendIn
    }

    connections UART {
      rpiDemo.UartBuffers -> uartBufferManager.bufferSendIn
      rpiDemo.UartWrite -> uartDrv.$send
      uartDrv.$recv -> rpiDemo.UartRead
      uartDrv.allocate -> uartBufferManager.bufferGetCallee
    }

    connections Uplink {
      comm.$recv -> frameAccumulator.dataIn

      frameAccumulator.frameOut -> deframer.framedIn
      deframer.deframedOut -> uplinkRouter.dataIn

      uplinkRouter.commandOut -> cmdDisp.seqCmdBuff
      uplinkRouter.fileOut -> fileUplink.bufferSendIn

      cmdDisp.seqCmdStatus -> uplinkRouter.cmdResponseIn
    }

  }

}
