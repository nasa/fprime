module RPI {

  topology RPI {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance $health
    instance chanTlm
    instance cmdDisp
    instance cmdSeq
    instance comQueue
    instance comDriver
    instance comStub
    instance deframer
    instance framer
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
    instance fprimeRouter

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
      eventLogger.PktSend -> comQueue.comPacketQueueIn[0]
      chanTlm.PktSend -> comQueue.comPacketQueueIn[1]
      fileDownlink.bufferSendOut -> comQueue.bufferQueueIn[0]

      comQueue.queueSend -> framer.dataIn
      comQueue.bufferReturnOut[0] -> fileDownlink.bufferReturn
      framer.dataReturnOut -> comQueue.bufferReturnIn

      framer.bufferAllocate -> commsBufferManager.bufferGetCallee
      framer.bufferDeallocate -> commsBufferManager.bufferSendIn

      framer.dataOut -> comStub.comDataIn
      comStub.dataReturnOut -> framer.dataReturnIn
      comDriver.dataReturnOut -> comStub.dataReturnIn

      comDriver.ready -> comStub.drvConnected
      comStub.drvDataOut -> comDriver.$send

      comStub.comStatusOut -> framer.comStatusIn
      framer.comStatusOut -> comQueue.comStatusIn
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

    connections MemoryAllocations {
      comDriver.allocate -> commsBufferManager.bufferGetCallee
      fileUplink.bufferSendOut -> commsBufferManager.bufferSendIn
      frameAccumulator.bufferAllocate -> commsBufferManager.bufferGetCallee
      frameAccumulator.bufferDeallocate -> commsBufferManager.bufferSendIn
      fprimeRouter.bufferDeallocate -> commsBufferManager.bufferSendIn
      deframer.bufferDeallocate -> commsBufferManager.bufferSendIn
    }

    connections UART {
      rpiDemo.UartBuffers -> uartBufferManager.bufferSendIn
      rpiDemo.UartWrite -> uartDrv.$send
      uartDrv.$recv -> rpiDemo.UartRead
      uartDrv.allocate -> uartBufferManager.bufferGetCallee
      uartDrv.dataReturnOut -> rpiDemo.UartWriteReturn
    }

    connections Uplink {
      comDriver.$recv -> comStub.drvDataIn
      comStub.comDataOut -> frameAccumulator.dataIn

      frameAccumulator.frameOut -> deframer.framedIn
      deframer.deframedOut -> fprimeRouter.dataIn

      fprimeRouter.commandOut -> cmdDisp.seqCmdBuff
      fprimeRouter.fileOut -> fileUplink.bufferSendIn

      cmdDisp.seqCmdStatus -> fprimeRouter.cmdResponseIn
    }

  }

}
