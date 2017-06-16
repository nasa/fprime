		 
# overall targets

all: bin

bin: $(DEPLOYMENT_BINS)

clean: $(DEPLOYMENT_CLEANS)

ac_lvl1: $(foreach deployment,$(DEPLOYMENTS),$(AC_SERIALIZED_TARGETS_$(deployment)))
ac_lvl2: $(foreach deployment,$(DEPLOYMENTS),$(AC_PORTS_TARGETS_$(deployment)))
ac_lvl3: $(foreach deployment,$(DEPLOYMENTS),$(AC_COMPONENTS_TARGETS_$(deployment)))

clean_all: clean

# Module targets

BLD_LVL1_TARGET_$(MOD_TARGET) : $(SRC_AC_SERIALIZABLE_$(MOD_TARGET))
BLD_LVL2_TARGET_$(MOD_TARGET) : $(SRC_AC_PORT_$(MOD_TARGET))
BLD_LVL3_TARGET_$(MOD_TARGET) : $(SRC_AC_COMPONENT_$(MOD_TARGET))

