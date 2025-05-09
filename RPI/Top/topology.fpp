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
      eventLogger.PktSend         -> comQueue.comPacketQueueIn[0]
      chanTlm.PktSend             -> comQueue.comPacketQueueIn[1]
      fileDownlink.bufferSendOut  -> comQueue.bufferQueueIn[0]
      comQueue.bufferReturnOut[0] -> fileDownlink.bufferReturn

      comQueue.dataOut     -> framer.dataIn
      framer.dataReturnOut -> comQueue.dataReturnIn

      framer.bufferAllocate   -> commsBufferManager.bufferGetCallee
      framer.bufferDeallocate -> commsBufferManager.bufferSendIn

      framer.dataOut          -> comStub.dataIn
      comStub.dataReturnOut   -> framer.dataReturnIn

      comStub.drvSendOut      -> comDriver.$send
      comDriver.sendReturnOut -> comStub.drvSendReturnIn
      comDriver.ready         -> comStub.drvConnected

      comStub.comStatusOut -> framer.comStatusIn
      framer.comStatusOut  -> comQueue.comStatusIn
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

    connections UART {
      rpiDemo.UartBuffers -> uartBufferManager.bufferSendIn
      rpiDemo.UartWrite -> uartDrv.$send
      uartDrv.$recv -> rpiDemo.UartRead
      uartDrv.allocate -> uartBufferManager.bufferGetCallee
      uartDrv.sendReturnOut -> rpiDemo.UartWriteReturn
    }

    connections Uplink {
      # ComDriver buffer allocations
      comDriver.allocate      -> commsBufferManager.bufferGetCallee
      comDriver.deallocate    -> commsBufferManager.bufferSendIn
      # ComDriver <-> ComStub
      comDriver.$recv             -> comStub.drvReceiveIn
      comStub.drvReceiveReturnOut -> comDriver.recvReturnIn
      # ComStub <-> FrameAccumulator
      comStub.dataOut                -> frameAccumulator.dataIn
      frameAccumulator.dataReturnOut -> comStub.dataReturnIn
      # FrameAccumulator buffer allocations
      frameAccumulator.bufferDeallocate -> commsBufferManager.bufferSendIn
      frameAccumulator.bufferAllocate   -> commsBufferManager.bufferGetCallee
      # FrameAccumulator <-> Deframer
      frameAccumulator.dataOut  -> deframer.dataIn
      deframer.dataReturnOut    -> frameAccumulator.dataReturnIn
      # Deframer <-> Router
      deframer.dataOut           -> fprimeRouter.dataIn
      fprimeRouter.dataReturnOut -> deframer.dataReturnIn
      # Router buffer allocations
      fprimeRouter.bufferAllocate   -> commsBufferManager.bufferGetCallee
      fprimeRouter.bufferDeallocate -> commsBufferManager.bufferSendIn
      # Router <-> CmdDispatcher/FileUplink
      fprimeRouter.commandOut  -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus     -> fprimeRouter.cmdResponseIn
      fprimeRouter.fileOut     -> fileUplink.bufferSendIn
      fileUplink.bufferSendOut -> fprimeRouter.fileBufferReturnIn
    }

  }

}
