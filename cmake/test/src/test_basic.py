####
# test_basic.py:
#
# Basic CMake tests.
#
####
import os
import platform
import cmake

# Test a normal build, with the ref executable and static libraries
BUILD_DIR = os.path.join("<FPRIME>", "Ref")
EXPECTED = [
    os.path.join("bin", platform.system(), "Ref"),
    os.path.join("<FPRIME>", "Ref", "Top", "RefTopologyAppDictionary.xml"),
    os.path.join("lib", platform.system(), "libDrv_BlockDriver.a"),
    os.path.join("lib", platform.system(), "libSvc_FileUplink.a"),
    os.path.join("lib", platform.system(), "libSvc_CmdSequencer.a"),
    os.path.join("lib", platform.system(), "libFw_Time.a"),
    os.path.join("lib", platform.system(), "libSvc_PassiveTextLogger.a"),
    os.path.join("lib", platform.system(), "libSvc_FileManager.a"),
    os.path.join("lib", platform.system(), "libSvc_PolyDb.a"),
    os.path.join("lib", platform.system(), "libSvc_SocketGndIf.a"),
    os.path.join("lib", platform.system(), "libRef_SendBuffApp.a"),
    os.path.join("lib", platform.system(), "libFw_FilePacket.a"),
    os.path.join("lib", platform.system(), "libSvc_Ping.a"),
    os.path.join("lib", platform.system(), "libFw_Types.a"),
    os.path.join("lib", platform.system(), "libUtils_Hash.a"),
    os.path.join("lib", platform.system(), "libFw_Port.a"),
    os.path.join("lib", platform.system(), "libSvc_RateGroupDriver.a"),
    os.path.join("lib", platform.system(), "libSvc_FatalHandler.a"),
    os.path.join("lib", platform.system(), "libSvc_TlmChan.a"),
    os.path.join("lib", platform.system(), "libFw_Obj.a"),
    os.path.join("lib", platform.system(), "libFw_Prm.a"),
    os.path.join("lib", platform.system(), "libSvc_ActiveRateGroup.a"),
    os.path.join("lib", platform.system(), "libSvc_Cycle.a"),
    os.path.join("lib", platform.system(), "libSvc_Health.a"),
    os.path.join("lib", platform.system(), "libSvc_LinuxTime.a"),
    os.path.join("lib", platform.system(), "libRef_PingReceiver.a"),
    os.path.join("lib", platform.system(), "libFw_Com.a"),
    os.path.join("lib", platform.system(), "libDrv_DataTypes.a"),
    os.path.join("lib", platform.system(), "libSvc_ComLogger.a"),
    os.path.join("lib", platform.system(), "libFw_SerializableFile.a"),
    os.path.join("lib", platform.system(), "libFw_Buffer.a"),
    os.path.join("lib", platform.system(), "libFw_Comp.a"),
    os.path.join("lib", platform.system(), "libSvc_CmdDispatcher.a"),
    os.path.join("lib", platform.system(), "libFw_Log.a"),
    os.path.join("lib", platform.system(), "libFw_Tlm.a"),
    os.path.join("lib", platform.system(), "libOs.a"),
    os.path.join("lib", platform.system(), "libRef_SignalGen.a"),
    os.path.join("lib", platform.system(), "libSvc_Sched.a"),
    os.path.join("lib", platform.system(), "libSvc_Seq.a"),
    os.path.join("lib", platform.system(), "libSvc_WatchDog.a"),
    os.path.join("lib", platform.system(), "libSvc_AssertFatalAdapter.a"),
    os.path.join("lib", platform.system(), "libSvc_PolyIf.a"),
    os.path.join("lib", platform.system(), "libSvc_FileDownlink.a"),
    os.path.join("lib", platform.system(), "libFw_ComFile.a"),
    os.path.join("lib", platform.system(), "libRef_RecvBuffApp.a"),
    os.path.join("lib", platform.system(), "libcodegen.a"),
    os.path.join("lib", platform.system(), "libSvc_Time.a"),
    os.path.join("lib", platform.system(), "libSvc_GndIf.a"),
    os.path.join("lib", platform.system(), "libFw_Cfg.a"),
    os.path.join("lib", platform.system(), "libSvc_Fatal.a"),
    os.path.join("lib", platform.system(), "libSvc_ActiveLogger.a"),
    os.path.join("lib", platform.system(), "libSvc_BufferManager.a"),
    os.path.join("lib", platform.system(), "libFw_Cmd.a"),
    os.path.join("lib", platform.system(), "libSvc_BuffGndSockIf.a"),
    os.path.join("lib", platform.system(), "libCFDP_Checksum.a"),
    os.path.join("lib", platform.system(), "libSvc_PrmDb.a"),
    os.path.join("lib", platform.system(), "libSvc_PassiveConsoleTextLogger.a"),
]
cmake.register_test(__name__, "ref-standard")
