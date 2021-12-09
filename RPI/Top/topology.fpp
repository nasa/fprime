module RPI {

  topology RPI {

    instance fatalHandler

    instance rateGroup10HzComp

    instance rateGroup1HzComp

    instance chanTlm

    instance cmdDisp

    instance prmDb

    instance cmdSeq

    instance fileUplink

    instance fileUplinkBufferManager

    instance fatalAdapter

    instance $health

    instance staticMemory

    instance downlink

    instance uplink

    instance comm

    instance eventLogger

    instance linuxTime

    instance linuxTimer

    instance rateGroupDriverComp

    instance fileDownlink

    instance textLogger

    instance uartDrv

    instance ledDrv

    instance gpio23Drv

    instance gpio24Drv

    instance gpio25Drv

    instance gpio17Drv

    instance spiDrv

    instance rpiDemo

    connections XML {
      comm.allocate[0] -> staticMemory.bufferAllocate[0]
      comm.$recv[0] -> uplink.framedIn[0]
      uplink.framedDeallocate[0] -> staticMemory.bufferDeallocate[0]
      uplink.bufferAllocate[0] -> fileUplinkBufferManager.bufferGetCallee[0]
      uplink.comOut[0] -> cmdDisp.seqCmdBuff[0]
      uplink.bufferOut[0] -> fileUplink.bufferSendIn[0]
      uplink.bufferDeallocate[0] -> fileUplinkBufferManager.bufferSendIn[0]
      fileUplink.bufferSendOut[0] -> fileUplinkBufferManager.bufferSendIn[0]
      downlink.framedAllocate[0] -> staticMemory.bufferAllocate[1]
      downlink.framedOut[0] -> comm.send[0]
      comm.deallocate[0] -> staticMemory.bufferDeallocate[1]
      eventLogger.PktSend[0] -> downlink.comIn[0]
      chanTlm.PktSend[0] -> downlink.comIn[0]
      fileDownlink.bufferSendOut[0] -> downlink.bufferIn[0]
      downlink.bufferDeallocate[0] -> fileDownlink.bufferReturn[0]
      eventLogger.CmdReg[0] -> cmdDisp.compCmdReg[0]
      cmdDisp.CmdReg[0] -> cmdDisp.compCmdReg[1]
      $health.CmdReg[0] -> cmdDisp.compCmdReg[2]
      prmDb.CmdReg[0] -> cmdDisp.compCmdReg[3]
      cmdSeq.cmdRegOut[0] -> cmdDisp.compCmdReg[4]
      fileDownlink.cmdRegOut[0] -> cmdDisp.compCmdReg[5]
      rpiDemo.cmdRegOut[0] -> cmdDisp.compCmdReg[6]
      cmdDisp.compCmdSend[0] -> eventLogger.CmdDisp[0]
      cmdDisp.compCmdSend[1] -> cmdDisp.CmdDisp[0]
      cmdDisp.compCmdSend[2] -> $health.CmdDisp[0]
      cmdDisp.compCmdSend[3] -> prmDb.CmdDisp[0]
      cmdDisp.compCmdSend[4] -> cmdSeq.cmdIn[0]
      cmdDisp.compCmdSend[5] -> fileDownlink.cmdIn[0]
      cmdDisp.compCmdSend[6] -> rpiDemo.cmdIn[0]
      eventLogger.CmdStatus[0] -> cmdDisp.compCmdStat[0]
      cmdSeq.cmdResponseOut[0] -> cmdDisp.compCmdStat[0]
      prmDb.CmdStatus[0] -> cmdDisp.compCmdStat[0]
      cmdDisp.CmdStatus[0] -> cmdDisp.compCmdStat[0]
      $health.CmdStatus[0] -> cmdDisp.compCmdStat[0]
      fileDownlink.cmdResponseOut[0] -> cmdDisp.compCmdStat[0]
      rpiDemo.cmdResponseOut[0] -> cmdDisp.compCmdStat[0]
      cmdDisp.seqCmdStatus[1] -> cmdSeq.cmdResponseIn[0]
      cmdSeq.comCmdOut[0] -> cmdDisp.seqCmdBuff[1]
      prmDb.Log[0] -> eventLogger.LogRecv[0]
      fileUplink.eventOut[0] -> eventLogger.LogRecv[0]
      eventLogger.Log[0] -> eventLogger.LogRecv[0]
      fileUplinkBufferManager.eventOut[0] -> eventLogger.LogRecv[0]
      cmdDisp.Log[0] -> eventLogger.LogRecv[0]
      cmdSeq.logOut[0] -> eventLogger.LogRecv[0]
      fatalAdapter.Log[0] -> eventLogger.LogRecv[0]
      $health.Log[0] -> eventLogger.LogRecv[0]
      fileDownlink.eventOut[0] -> eventLogger.LogRecv[0]
      rateGroup10HzComp.Log[0] -> eventLogger.LogRecv[0]
      rateGroup1HzComp.Log[0] -> eventLogger.LogRecv[0]
      rpiDemo.eventOut[0] -> eventLogger.LogRecv[0]
      uartDrv.Log[0] -> eventLogger.LogRecv[0]
      spiDrv.Log[0] -> eventLogger.LogRecv[0]
      ledDrv.Log[0] -> eventLogger.LogRecv[0]
      gpio23Drv.Log[0] -> eventLogger.LogRecv[0]
      gpio24Drv.Log[0] -> eventLogger.LogRecv[0]
      gpio25Drv.Log[0] -> eventLogger.LogRecv[0]
      gpio17Drv.Log[0] -> eventLogger.LogRecv[0]
      cmdSeq.LogText[0] -> textLogger.TextLogger[0]
      eventLogger.LogText[0] -> textLogger.TextLogger[0]
      $health.LogText[0] -> textLogger.TextLogger[0]
      fatalAdapter.LogText[0] -> textLogger.TextLogger[0]
      rateGroup10HzComp.LogText[0] -> textLogger.TextLogger[0]
      rateGroup1HzComp.LogText[0] -> textLogger.TextLogger[0]
      prmDb.LogText[0] -> textLogger.TextLogger[0]
      cmdDisp.LogText[0] -> textLogger.TextLogger[0]
      rpiDemo.textEventOut[0] -> textLogger.TextLogger[0]
      uartDrv.LogText[0] -> textLogger.TextLogger[0]
      spiDrv.LogText[0] -> textLogger.TextLogger[0]
      ledDrv.LogText[0] -> textLogger.TextLogger[0]
      gpio23Drv.LogText[0] -> textLogger.TextLogger[0]
      gpio24Drv.LogText[0] -> textLogger.TextLogger[0]
      gpio25Drv.LogText[0] -> textLogger.TextLogger[0]
      gpio17Drv.LogText[0] -> textLogger.TextLogger[0]
      fileDownlink.tlmOut[0] -> chanTlm.TlmRecv[0]
      fileUplinkBufferManager.tlmOut[0] -> chanTlm.TlmRecv[0]
      fileUplink.tlmOut[0] -> chanTlm.TlmRecv[0]
      cmdSeq.tlmOut[0] -> chanTlm.TlmRecv[0]
      cmdDisp.Tlm[0] -> chanTlm.TlmRecv[0]
      rateGroup10HzComp.Tlm[0] -> chanTlm.TlmRecv[0]
      $health.Tlm[0] -> chanTlm.TlmRecv[0]
      rpiDemo.tlmOut[0] -> chanTlm.TlmRecv[0]
      uartDrv.Tlm[0] -> chanTlm.TlmRecv[0]
      spiDrv.Tlm[0] -> chanTlm.TlmRecv[0]
      rpiDemo.prmGetOut[0] -> prmDb.getPrm[0]
      rpiDemo.prmSetOut[0] -> prmDb.setPrm[0]
      prmDb.Time[0] -> linuxTime.timeGetPort[0]
      eventLogger.Time[0] -> linuxTime.timeGetPort[0]
      rateGroup10HzComp.Time[0] -> linuxTime.timeGetPort[0]
      cmdSeq.timeCaller[0] -> linuxTime.timeGetPort[0]
      $health.Time[0] -> linuxTime.timeGetPort[0]
      fileUplinkBufferManager.timeCaller[0] -> linuxTime.timeGetPort[0]
      cmdDisp.Time[0] -> linuxTime.timeGetPort[0]
      fileUplink.timeCaller[0] -> linuxTime.timeGetPort[0]
      fileDownlink.timeCaller[0] -> linuxTime.timeGetPort[0]
      fatalAdapter.Time[0] -> linuxTime.timeGetPort[0]
      rpiDemo.timeGetOut[0] -> linuxTime.timeGetPort[0]
      uartDrv.Time[0] -> linuxTime.timeGetPort[0]
      spiDrv.Time[0] -> linuxTime.timeGetPort[0]
      ledDrv.Time[0] -> linuxTime.timeGetPort[0]
      gpio23Drv.Time[0] -> linuxTime.timeGetPort[0]
      gpio24Drv.Time[0] -> linuxTime.timeGetPort[0]
      gpio25Drv.Time[0] -> linuxTime.timeGetPort[0]
      gpio17Drv.Time[0] -> linuxTime.timeGetPort[0]
      linuxTimer.CycleOut[0] -> rateGroupDriverComp.CycleIn[0]
      rateGroupDriverComp.CycleOut[0] -> rateGroup10HzComp.CycleIn[0]
      rateGroup10HzComp.RateGroupMemberOut[0] -> rpiDemo.Run[0]
      rateGroup10HzComp.RateGroupMemberOut[1] -> fileDownlink.Run[0]
      rateGroupDriverComp.CycleOut[1] -> rateGroup1HzComp.CycleIn[0]
      rateGroup1HzComp.RateGroupMemberOut[0] -> chanTlm.Run[0]
      rateGroup1HzComp.RateGroupMemberOut[1] -> $health.Run[0]
      rateGroup1HzComp.RateGroupMemberOut[2] -> rpiDemo.Run[0]
      rateGroup1HzComp.RateGroupMemberOut[3] -> cmdSeq.schedIn[0]
      # rateGroup3Comp.RateGroupMemberOut[2] -> fileUplinkBufferManager.schedIn[0]
      $health.PingSend[0] -> rateGroup10HzComp.PingIn[0]
      rateGroup10HzComp.PingOut[0] -> $health.PingReturn[0]
      $health.PingSend[1] -> rateGroup1HzComp.PingIn[0]
      rateGroup1HzComp.PingOut[0] -> $health.PingReturn[1]
      $health.PingSend[2] -> cmdDisp.pingIn[0]
      cmdDisp.pingOut[0] -> $health.PingReturn[2]
      $health.PingSend[3] -> cmdSeq.pingIn[0]
      cmdSeq.pingOut[0] -> $health.PingReturn[3]
      $health.PingSend[4] -> chanTlm.pingIn[0]
      chanTlm.pingOut[0] -> $health.PingReturn[4]
      $health.PingSend[5] -> eventLogger.pingIn[0]
      eventLogger.pingOut[0] -> $health.PingReturn[5]
      $health.PingSend[6] -> prmDb.pingIn[0]
      prmDb.pingOut[0] -> $health.PingReturn[6]
      $health.PingSend[7] -> fileDownlink.pingIn[0]
      fileDownlink.pingOut[0] -> $health.PingReturn[7]
      $health.PingSend[8] -> fileUplink.pingIn[0]
      fileUplink.pingOut[0] -> $health.PingReturn[8]
      eventLogger.FatalAnnounce[0] -> fatalHandler.FatalReceive[0]
      rpiDemo.GpioWrite[2] -> ledDrv.gpioWrite[0]
      rpiDemo.GpioWrite[0] -> gpio23Drv.gpioWrite[0]
      rpiDemo.GpioWrite[1] -> gpio24Drv.gpioWrite[0]
      rpiDemo.GpioRead[0] -> gpio25Drv.gpioRead[0]
      rpiDemo.GpioRead[1] -> gpio17Drv.gpioRead[0]
      rpiDemo.UartWrite[0] -> uartDrv.serialSend[0]
      uartDrv.serialRecv[0] -> rpiDemo.UartRead[0]
      rpiDemo.UartBuffers[0] -> uartDrv.readBufferSend[0]
      rpiDemo.SpiReadWrite[0] -> spiDrv.SpiReadWrite[0]
      cmdDisp.seqCmdStatus -> uplink.cmdResponseIn
    }

  }

}
