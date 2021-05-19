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
	Fw/Types \
    Fw/Logger

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
	Utils/Hash \
	Utils/Types
        
SVC_MODULES := \
	Svc/BufferAccumulator \
	Svc/BufferLogger \
	Svc/BufferManager \
	Svc/CmdDispatcher \
	Svc/CmdSequencer \
	Svc/Seq \
	Svc/ActiveRateGroup \
	Svc/RateGroupDriver \
	Svc/Sched \
	Svc/ComLogger \
	Svc/TlmChan \
	Svc/PassiveConsoleTextLogger \
	Svc/Time \
	Svc/Cycle \
	Svc/LinuxTime \
	Svc/LinuxTimer \
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
	Svc/FileManager \
	Svc/UdpSender \
	Svc/UdpReceiver \
	Svc/GroundInterface
	

DEMO_DRV_MODULES := \
	Drv/DataTypes \
	Drv/BlockDriver \
	Drv/GpioDriverPorts \
	Drv/SocketIpDriver
		
LINUX_DRV_MODULES := \
	Drv/LinuxGpioDriver \
	Drv/LinuxSerialDriver \
	Drv/LinuxSpiDriver \
	Drv/SerialDriverPorts \
	Drv/SpiDriverPorts \
	Drv/GpioDriverPorts
	
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
	$(DEMO_DRV_MODULES) \
	\
	$(FW_MODULES) \
	\
	$(OS_MODULES) \
	\
	$(CFDP_MODULES) \
  	\
  	$(UTILS_MODULES)
		
ACDEVTEST_MODULES := \
	Autocoders/Python/test/active_tester \
	Autocoders/Python/test/app1 \
	Autocoders/Python/test/app2 \
	Autocoders/Python/test/cnt_only \
	Autocoders/Python/test/command1 \
	Autocoders/Python/test/command2 \
	Autocoders/Python/test/command_res \
	Autocoders/Python/test/command_multi_inst \
	Autocoders/Python/test/command_string \
	Autocoders/Python/test/command_tester \
	Autocoders/Python/test/comp_diff_namespace \
	Autocoders/Python/test/comp_no_namespace \
	Autocoders/Python/test/enum1port \
	Autocoders/Python/test/enum_return_port \
	Autocoders/Python/test/event1 \
	Autocoders/Python/test/event2 \
	Autocoders/Python/test/event_throttle \
	Autocoders/Python/test/event_enum \
	Autocoders/Python/test/event_multi_inst \
	Autocoders/Python/test/event_string \
	Autocoders/Python/test/ext_dict \
	Autocoders/Python/test/log1 \
	Autocoders/Python/test/log_tester \
	Autocoders/Python/test/main \
	Autocoders/Python/test/noargport \
	Autocoders/Python/test/param1 \
	Autocoders/Python/test/param2 \
	Autocoders/Python/test/param_enum \
	Autocoders/Python/test/param_multi_inst \
	Autocoders/Python/test/param_string \
	Autocoders/Python/test/param_tester \
	Autocoders/Python/test/time_tester \
	#Autocoders/Python/test/queued1 \
	\ # Autocoders/Python/test/partition \
	Autocoders/Python/test/pass_by_attrib \
	\ # Autocoders/Python/test/passive \
	Autocoders/Python/test/port_nogen \
	Autocoders/Python/test/port_return_type \
	Autocoders/Python/test/serialize_enum \
	Autocoders/Python/test/serialize_stringbuffer \
	Autocoders/Python/test/serialize_template \
	Autocoders/Python/test/serialize_user \
	Autocoders/Python/test/serialize1 \
	Autocoders/Python/test/serialize2 \
	Autocoders/Python/test/serialize3 \
	Autocoders/Python/test/stress \
	Autocoders/Python/test/string_port \
	Autocoders/Python/test/telem_tester \
	Autocoders/Python/test/tlm_enum \
	Autocoders/Python/test/tlm_string \
	Autocoders/Python/test/tlm1 \
	Autocoders/Python/test/tlm2 \
	Autocoders/Python/test/tlm_onchange \
	Autocoders/Python/test/tlm_multi_inst \
	Autocoders/Python/test/interface1 \
	Autocoders/Python/test/port_loopback \
	Autocoders/Python/test/serial_passive \
    \
	Autocoders/Python/templates
	
RPI_APP_MODULES := \
	RPI/Top \
	RPI/RpiDemo
	
RPI_MODULES := \
	\
	$(RPI_APP_MODULES) \
	\
	$(SVC_MODULES) \
	\
	$(LINUX_DRV_MODULES) \
	\
	$(CFDP_MODULES) \
  	\
	$(FW_MODULES) \
	\
	$(OS_MODULES) \
	\
  	$(UTILS_MODULES)

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
	STest \
	$(FW_GTEST_MODULES)

# List deployments

DEPLOYMENTS := Ref acdev RPI

