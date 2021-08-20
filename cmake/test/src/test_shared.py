####
# test_shared.py:
#
# CMake tests to ensure that shared libraries are output as part of the build.
#
####
import os
import platform
import cmake

ext = "so"
if platform.system() == "Windows":
    ext = "dll"
elif platform.system() == "Darwin":
    ext = "dylib"

# Test a normal build, with the ref executable and static libraries
BUILD_DIR = os.path.join("<FPRIME>", "Ref")
OPTIONS = {"BUILD_SHARED_LIBS": "ON"}
EXPECTED = [
    os.path.join("bin", platform.system(), "Ref"),
    os.path.join("<FPRIME>", "Ref", "Top", "RefTopologyAppDictionary.xml"),
    os.path.join("lib", platform.system(), "libDrv_BlockDriver." + ext),
    os.path.join("lib", platform.system(), "libSvc_FileUplink." + ext),
    os.path.join("lib", platform.system(), "libSvc_CmdSequencer." + ext),
    os.path.join("lib", platform.system(), "libFw_Time." + ext),
    os.path.join("lib", platform.system(), "libSvc_FileManager." + ext),
    os.path.join("lib", platform.system(), "libSvc_PolyDb." + ext),
    os.path.join("lib", platform.system(), "libRef_SendBuffApp." + ext),
    os.path.join("lib", platform.system(), "libFw_FilePacket." + ext),
    os.path.join("lib", platform.system(), "libSvc_Ping." + ext),
    os.path.join("lib", platform.system(), "libFw_Types." + ext),
    os.path.join("lib", platform.system(), "libUtils_Hash." + ext),
    os.path.join("lib", platform.system(), "libFw_Port." + ext),
    os.path.join("lib", platform.system(), "libSvc_RateGroupDriver." + ext),
    os.path.join("lib", platform.system(), "libSvc_FatalHandler." + ext),
    os.path.join("lib", platform.system(), "libSvc_TlmChan." + ext),
    os.path.join("lib", platform.system(), "libFw_Obj." + ext),
    os.path.join("lib", platform.system(), "libFw_Prm." + ext),
    os.path.join("lib", platform.system(), "libSvc_ActiveRateGroup." + ext),
    os.path.join("lib", platform.system(), "libSvc_Cycle." + ext),
    os.path.join("lib", platform.system(), "libSvc_Health." + ext),
    os.path.join("lib", platform.system(), "libSvc_LinuxTime." + ext),
    os.path.join("lib", platform.system(), "libRef_PingReceiver." + ext),
    os.path.join("lib", platform.system(), "libFw_Com." + ext),
    os.path.join("lib", platform.system(), "libDrv_DataTypes." + ext),
    os.path.join("lib", platform.system(), "libSvc_ComLogger." + ext),
    os.path.join("lib", platform.system(), "libFw_SerializableFile." + ext),
    os.path.join("lib", platform.system(), "libFw_Buffer." + ext),
    os.path.join("lib", platform.system(), "libFw_Comp." + ext),
    os.path.join("lib", platform.system(), "libSvc_CmdDispatcher." + ext),
    os.path.join("lib", platform.system(), "libFw_Log." + ext),
    os.path.join("lib", platform.system(), "libFw_Tlm." + ext),
    os.path.join("lib", platform.system(), "libOs." + ext),
    os.path.join("lib", platform.system(), "libRef_SignalGen." + ext),
    os.path.join("lib", platform.system(), "libSvc_Sched." + ext),
    os.path.join("lib", platform.system(), "libSvc_Seq." + ext),
    os.path.join("lib", platform.system(), "libSvc_WatchDog." + ext),
    os.path.join("lib", platform.system(), "libSvc_AssertFatalAdapter." + ext),
    os.path.join("lib", platform.system(), "libSvc_PolyIf." + ext),
    os.path.join("lib", platform.system(), "libSvc_FileDownlink." + ext),
    os.path.join("lib", platform.system(), "libRef_RecvBuffApp." + ext),
    os.path.join("lib", platform.system(), "libSvc_Time." + ext),
    os.path.join("lib", platform.system(), "libFw_Cfg." + ext),
    os.path.join("lib", platform.system(), "libSvc_Fatal." + ext),
    os.path.join("lib", platform.system(), "libSvc_ActiveLogger." + ext),
    os.path.join("lib", platform.system(), "libSvc_BufferManager." + ext),
    os.path.join("lib", platform.system(), "libFw_Cmd." + ext),
    os.path.join("lib", platform.system(), "libCFDP_Checksum." + ext),
    os.path.join("lib", platform.system(), "libSvc_PrmDb." + ext),
    os.path.join("lib", platform.system(), "libSvc_PassiveConsoleTextLogger." + ext),
]
cmake.register_test(__name__, "ref-shared")
