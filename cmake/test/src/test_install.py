####
# test_install.py:
#
# CMake install test.
####
import os
import cmake

# Test a normal build, with the ref executable and static libraries
BUILD_DIR = os.path.join("<FPRIME>", "Ref")
EXPECTED = [
    os.path.join("<FPRIME_INSTALL>", "bin", "Ref"),
    os.path.join("<FPRIME_INSTALL>", "dict", "RefTopologyAppDictionary.xml"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libDrv_BlockDriver.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_FileUplink.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_CmdSequencer.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Time.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_FileManager.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libRef_SendBuffApp.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_FilePacket.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_Ping.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Types.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libUtils_Hash.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Port.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_RateGroupDriver.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_FatalHandler.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_TlmChan.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Obj.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Prm.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_ActiveRateGroup.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_Cycle.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_Health.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_LinuxTime.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libRef_PingReceiver.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Com.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libDrv_DataTypes.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Buffer.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Comp.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_CmdDispatcher.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Log.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Tlm.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libOs.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libRef_SignalGen.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_Sched.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_Seq.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_WatchDog.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_AssertFatalAdapter.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_FileDownlink.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libRef_RecvBuffApp.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_Time.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Cfg.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_Fatal.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_ActiveLogger.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_BufferManager.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libFw_Cmd.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libCFDP_Checksum.a"),
    os.path.join("<FPRIME_INSTALL>", "lib", "static", "libSvc_PrmDb.a"),
    os.path.join(
        "<FPRIME_INSTALL>",
        "lib",
        "static",
        "libSvc_PassiveConsoleTextLogger.a",
    ),
]
TARGETS = ["package_gen"]
cmake.register_test(__name__, "install-test")
