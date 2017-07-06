# List of flight software modules to build

# NOTE: Must not be characters after continuation character "\" or will get "commands commence before first target" errors

FW_MODULES := \
	Fw/FilePacket \
	Fw/Cfg \
	Fw/Buffer \
	Fw/Comp \
	Fw/Obj \
	Fw/Port \
	Fw/Cmd \
	Fw/Tlm \
	Fw/Prm \
	Fw/Log \
	Fw/Time \
	Fw/Com \
	Fw/ComFile \
	Fw/SerializableFile \
	Fw/Types

FW_GTEST_MODULES := \
	Fw/SerializableFile/test/TestSerializable \
	Fw/FilePacket/GTest \
	Fw/Types/GTest
	
OS_MODULES := \
	Os 

CFDP_MODULES := \
	CFDP/Checksum

CFDP_GTEST_MODULES := \
	CFDP/Checksum/GTest
	
UTILS_MODULES := \
	Utils/Hash
        
SVC_MODULES := \
	Svc/BufferManager \
	Svc/CmdDispatcher \
	Svc/CmdSequencer \
	Svc/Seq \
	Svc/GndIf \
	Svc/ActiveRateGroup \
	Svc/RateGroupDriver \
	Svc/Sched \
	Svc/ComLogger \
	Svc/SocketGndIf \
	Svc/BuffGndSockIf \
	Svc/TlmChan \
	Svc/PassiveTextLogger \
	Svc/PassiveConsoleTextLogger \
	Svc/Time \
	Svc/Cycle \
	Svc/LinuxTime \
	Svc/ActiveLogger \
	Svc/Fatal \
	Svc/PolyIf \
	Svc/PolyDb \
	Svc/PrmDb \
	Svc/Ping \
	Svc/Health \
	Svc/WatchDog \
	Svc/FileUplink \
	Svc/FileDownlink \
    Svc/AssertFatalAdapter \
    Svc/FatalHandler \
	Svc/FileManager

DRV_MODULES := \
	Drv/DataTypes \
	Drv/BlockDriver
	

REF_MODULES := \
	Ref/Top \
	Ref/RecvBuffApp \
	Ref/SendBuffApp \
	Ref/SignalGen \
	Ref/PingReceiver
	
Ref_MODULES := \
	\
	$(REF_MODULES) \
	\
	$(SVC_MODULES) \
	\
	$(DRV_MODULES) \
	\
	$(FW_MODULES) \
	\
	$(OS_MODULES) \
	\
	$(CFDP_MODULES) \
  	\
  	$(UTILS_MODULES)
		
ACDEVTEST_MODULES := \
	Autocoders/test/active_tester \
	Autocoders/test/app1 \
	Autocoders/test/app2 \
	Autocoders/test/cnt_only \
	Autocoders/test/command1 \
	Autocoders/test/command2 \
	Autocoders/test/command_res \
	Autocoders/test/command_multi_inst \
	Autocoders/test/command_string \
	Autocoders/test/command_tester \
	Autocoders/test/comp_diff_namespace \
	Autocoders/test/comp_no_namespace \
	Autocoders/test/enum1port \
	Autocoders/test/enum_return_port \
	Autocoders/test/event1 \
	Autocoders/test/event2 \
	Autocoders/test/event_throttle \
	Autocoders/test/event_enum \
	Autocoders/test/event_multi_inst \
	Autocoders/test/event_string \
	Autocoders/test/ext_dict \
	Autocoders/test/log1 \
	Autocoders/test/log_tester \
	Autocoders/test/main \
	Autocoders/test/noargport \
	Autocoders/test/param1 \
	Autocoders/test/param2 \
	Autocoders/test/param_enum \
	Autocoders/test/param_multi_inst \
	Autocoders/test/param_string \
	Autocoders/test/param_tester \
	Autocoders/test/time_tester \
	#Autocoders/test/queued1 \
	\ # Autocoders/test/partition \
	Autocoders/test/pass_by_attrib \
	\ # Autocoders/test/passive \
	Autocoders/test/port_nogen \
	Autocoders/test/port_return_type \
	Autocoders/test/serialize_enum \
	Autocoders/test/serialize_stringbuffer \
	Autocoders/test/serialize_template \
	Autocoders/test/serialize_user \
	Autocoders/test/serialize1 \
	Autocoders/test/serialize2 \
	Autocoders/test/serialize3 \
	Autocoders/test/stress \
	Autocoders/test/string_port \
	Autocoders/test/telem_tester \
	Autocoders/test/tlm_enum \
	Autocoders/test/tlm_string \
	Autocoders/test/tlm1 \
	Autocoders/test/tlm2 \
	Autocoders/test/tlm_onchange \
	Autocoders/test/tlm_multi_inst \
	Autocoders/test/interface1 \
	Autocoders/test/port_loopback \
	Autocoders/test/serial_passive \
    \
	Autocoders/templates
	
acdev_MODULES := \
	$(FW_MODULES) \
	\
	$(OS_MODULES) \
	\
	$(ACDEVTEST_MODULES)
	
	
# Other modules to build, but not to link with deployment binaries
OTHER_MODULES := \
	gtest \
	Os/Stubs \
	Fw/Test \
	$(FW_GTEST_MODULES)

# List deployments

DEPLOYMENTS := Ref acdev

# Location of ground/gse software. Autocoded dictionary elements are copied here.
GDS_MODULE := Gse
