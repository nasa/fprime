include $(BUILD_ROOT)/mk/makefiles/build_vars.mk

# make variables

DEPLOYMENT_BINS : $(foreach deployment,$(DEPLOYMENTS),bin_$(deployment))
DEPLOYMENT_CLEANS : $(foreach deployment,$(DEPLOYMENTS),clean_$(deployment))
