"""
cmake/test/settings.py:

Settings for the CMake tests. Mostly constants that must be tracked.
"""
from pathlib import Path

DATA_DIR = Path(__file__).parent.parent / "data"
REF_APP_PATH = Path(__file__).parent.parent.parent.parent / "Ref"


FRAMEWORK_MODULES = [
    "Fw_Cfg",
    "Fw_Cmd",
    "Fw_Com",
    "Fw_Comp",
    "Fw_CompQueued",
    "Fw_Log",
    "Fw_Logger",
    "Fw_Obj",
    "Fw_Port",
    "Fw_Time",
    "Fw_Tlm",
    "Fw_Types",
    "Os",
]

STANDARD_MODULES = [
    "Fw_Buffer",
    "Fw_FilePacket",
    "Fw_Prm",
    "CFDP_Checksum",
    "Drv_ByteStreamDriverModel",
    "Drv_DataTypes",
    "Drv_Ip",
    "Drv_TcpClient",
    "Utils_Hash",
    "Utils_Types",
]

REF_MODULES = [
    "Svc_ActiveLogger",
    "Svc_ActiveRateGroup",
    "Svc_AssertFatalAdapter",
    "Svc_BufferManager",
    "Svc_CmdDispatcher",
    "Svc_CmdSequencer",
    "Svc_Cycle",
    "Svc_Deframer",
    "Svc_Fatal",
    "Svc_FatalHandler",
    "Svc_FileDownlink",
    "Svc_FileManager",
    "Svc_FileUplink",
    "Svc_Framer",
    "Svc_FramingProtocol",
    "Svc_Health",
    "Svc_PosixTime",
    "Svc_PassiveConsoleTextLogger",
    "Svc_Ping",
    "Svc_PrmDb",
    "Svc_RateGroupDriver",
    "Svc_Sched",
    "Svc_Seq",
    "Svc_StaticMemory",
    "Svc_Time",
    "Svc_TlmChan",
    "Drv_BlockDriver",
    "Ref_PingReceiver",
    "Ref_RecvBuffApp",
    "Ref_SendBuffApp",
    "Ref_SignalGen",
    "Ref_Top",
]
