####
# TestDeploymentsProject configuration overrides:
#
# Replaces framework default configuration files, matched by file name. CdhCoreTlmConfig.fpp selects
# Svc.TlmPacketizer as the CdhCore telemetry instance so that the Ref deployment exercises packetized
# telemetry and its data product recording.
####
register_fprime_config(
        TestDeploymentsProject_ConfigOverrides
    CONFIGURATION_OVERRIDES
        "${CMAKE_CURRENT_LIST_DIR}/CdhCoreTlmConfig.fpp"
    INTERFACE
)
