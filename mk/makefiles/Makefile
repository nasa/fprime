
include $(BUILD_ROOT)/mk/makefiles/build_vars.mk

include $(BUILD_ROOT)/mk/makefiles/vars.mk
include $(BUILD_ROOT)/mk/makefiles/ac_targets.mk
ifneq ("$(BUILD)","") # only include if building binary targets
   include $(BUILD_ROOT)/mk/makefiles/bin_targets.mk
endif   


# Global code generator targets

AC_LVL1  : $(foreach module,$($(DEPLOYMENT)_MODULES),$(SRC_AC_SERIALIZABLE_$(subst /,,$(module))))
AC_LVL2  : $(foreach module,$($(DEPLOYMENT)_MODULES),$(SRC_AC_PORT_$(subst /,,$(module))))
AC_LVL3  : $(foreach module,$($(DEPLOYMENT)_MODULES),$(SRC_AC_COMPONENT_$(subst /,,$(module))))
AC_LVL4  : $(foreach module,$($(DEPLOYMENT)_MODULES),$(SRC_AC_APP_$(subst /,,$(module))))
ALL_HDRS := $(foreach module,$($(DEPLOYMENT)_MODULES),$(HDR_$(subst /,,$(module))) $(HDR_AC_$(subst /,,$(module))))   

# Global binary targets

TARGET_LIBS := $(foreach module,$($(DEPLOYMENT)_MODULES),$(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/$(LIB_PREFIX)$(subst /,,$(module))$(LIB_SUFFIX))
CLEAN_TARGETS := $(foreach module,$($(DEPLOYMENT)_MODULES),$(subst /,,$(module))_clean)
BIN_CLEAN_TARGETS := $(foreach module,$($(DEPLOYMENT)_MODULES),$(subst /,,$(module))_bin_clean)
SLOC_TARGETS := $(foreach module,$($(DEPLOYMENT)_MODULES),$(subst /,,$(module))_sloc)

bin_echo:
	@echo "Building $(BUILD)"

bin: bin_echo $(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)/$(BIN_PREFIX)$(DEPLOYMENT)$(BIN_SUFFIX)

clean: $(CLEAN_TARGETS) ampcs_clean
	$(RM_DIR) $(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)
	
bin_clean: $(BIN_CLEAN_TARGETS) ampcs_clean
	$(RM_DIR) $(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)
	
sloc: sloc_build $(SLOC_TARGETS) sloc_report

sloc_report:
	@$(PYTHON_BIN) $(PROCESS_SLOC) -d "Handwritten" -o summary $(foreach module,$($(DEPLOYMENT)_MODULES),$(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/written_sloc.txt)
	@$(PYTHON_BIN) $(PROCESS_SLOC) -d "Autocoded  " -o summary $(foreach module,$($(DEPLOYMENT)_MODULES),$(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/ac_sloc.txt)
	@$(PYTHON_BIN) $(PROCESS_SLOC) -d "XML        " -o summary $(foreach module,$($(DEPLOYMENT)_MODULES),$(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/xml_sloc.txt)
	@$(PYTHON_BIN) $(PROCESS_SLOC) -d "Totals     " -o summary $(foreach module,$($(DEPLOYMENT)_MODULES),$(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/written_sloc.txt) $(foreach module,$($(DEPLOYMENT)_MODULES),$(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/ac_sloc.txt) $(foreach module,$($(DEPLOYMENT)_MODULES),$(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/xml_sloc.txt)

sloc_csv: sloc_build $(SLOC_TARGETS) sloc_csv_report

sloc_csv_report:
	@$(PYTHON_BIN) $(PROCESS_SLOC) -o csv_header 
	@$(foreach module,$($(DEPLOYMENT)_MODULES),$(PYTHON_BIN) $(PROCESS_SLOC) -o csv -d $(module) $(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/written_sloc.txt $(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/ac_sloc.txt $(BUILD_ROOT)/$(module)/$(OUTPUT_DIR)/xml_sloc.txt;)
	

# build LARS SLOC tool for native platform
sloc_build: $(BUILD_ROOT)/mk/bin/ncsl

$(BUILD_ROOT)/mk/bin/ncsl: $(BUILD_ROOT)/mk/tools/ncsl/ncsl.c
	$(CC) $(COMPILE_TO) $@ $^

# define binary target

ifneq ("$(BUILD)","") # only define if building binary targets

$(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)/$(BIN_PREFIX)$(DEPLOYMENT)$(BIN_SUFFIX): $(TARGET_LIBS)
	@$(MKDIR) $(@D)
	@echo Generating version...
	@$(GEN_VERSION) $(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)/version.c
	@$(CC) $(COMPILE_ONLY) $(COMPILE_TO) $(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)/version.o $(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)/version.c
	@echo Linking binary...
	@$(LINK_BIN) $(LINK_BIN_FLAGS) $(LINK_BIN_TO) $@ $(LIBS_START) $^ $(LINK_LIBS) $(BUILD_ROOT)/$(DEPLOYMENT)/$(OUTPUT_DIR)/version.o $(LIBS_END) 
	@$(POST_LINK_BIN) $(POST_LINK_PRE_ARGS) $@ $(POST_LINK_POST_ARGS) 
	@$(SIZE) $@
	@$(CKSUM_SH) $@ $@.$(CKSUM_EXT)

endif
# Module-level code generator targets

MOD_AC_LVL1 : $(SRC_AC_SERIALIZABLE_$(MOD_TARGET))
MOD_AC_LVL2 : $(SRC_AC_PORT_$(MOD_TARGET))
MOD_AC_LVL3 : $(SRC_AC_COMPONENT_$(MOD_TARGET))
MOD_AC_LVL4 : $(SRC_AC_APP_$(MOD_TARGET))

gen_dox:
	@echo -e "INPUT = $(ALL_HDRS)\n" > $(BUILD_ROOT)/$(DEPLOYMENT)/dox.txt
	@$(CAT) $(BUILD_ROOT)/mk/configs/doxygen/doxygen.txt >> $(BUILD_ROOT)/$(DEPLOYMENT)/dox.txt
	@$(CAT) $(BUILD_ROOT)/$(DEPLOYMENT)/docs/doxygen.txt >> $(BUILD_ROOT)/$(DEPLOYMENT)/dox.txt
	@cd $(BUILD_ROOT)/$(DEPLOYMENT) && $(DOXYGEN) $(BUILD_ROOT)/$(DEPLOYMENT)/dox.txt
	@$(RM) $(BUILD_ROOT)/$(DEPLOYMENT)/dox.txt

clean_dox:
	rm -rf $(BUILD_ROOT)/$(DEPLOYMENT)/doxygen

run: 
	@cd $(BUILD_ROOT)/$(DEPLOYMENT_DIR) && $(shell $(RUN_TERMINAL) $(BUILD_ROOT)/$(DEPLOYMENT_DIR)/$(DEPLOYMENT)/$(OUTPUT_DIR)/$(BIN_PREFIX)$(DEPLOYMENT)$(BIN_SUFFIX))
	
run_ut:
	@echo "Running test/ut/runtest_$(BUILD) with output dir $(OUTPUT_DIR)"
	test/ut/runtest_$(BUILD) $(OUTPUT_DIR)
	
cov:
	$(GCOV) -o $(OUTPUT_DIR) $(SRC_$(MOD_TARGET)_$(BUILD)) $(SRC_AC_$(MOD_TARGET)) > $(OUTPUT_DIR)/$(MOD_TARGET)_gcov.txt

comp_report_gen:
	@$(COMP_REPORT_GEN) $(foreach module,$($(DEPLOYMENT)_MODULES),$(wildcard $(BUILD_ROOT)/$(module)/*ComponentReport.txt))
	@$(CAT) ComponentReport.txt
	
dict_clean:
	@echo "Cleaning dictionary in $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict/serializable"
	@$(RM_DIR) $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict/serializable

dict_install: 
	@echo "Installing instance dictionaries into $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict"
	@$(MKDIR) $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict/serializable
	@$(TOUCH) $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict/serializable/__init__.py
	@echo "Installing serializables in $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict/serializable"
	@$(CP_RECURSE) $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict/serializable/__init__.py $(foreach module,$($(DEPLOYMENT)_MODULES),$(wildcard $(BUILD_ROOT)/$(module)/$(DICT_MODULE_SUBDIR)/serializable)) $(BUILD_ROOT)/$(DEPLOYMENT)/py_dict

gen_mod_sdds: $(foreach module,$($(DEPLOYMENT)_MODULES),$(subst /,,$(module))_sdd)

gen_dep_sdd: $(BUILD_ROOT)/$(DEPLOYMENT)/docs/sdd.html

$(BUILD_ROOT)/$(DEPLOYMENT)/docs/sdd.html: $(BUILD_ROOT)/$(DEPLOYMENT)/docs/sdd.md
	@echo "Generating $@ from $<"
	@$(CAT) $(BUILD_ROOT)/mk/docs/SDD.css > $@
	-@$(MARKDOWN) $< >> $@
	
dep_sdd_clean:
	@$(RM) $(BUILD_ROOT)/$(DEPLOYMENT)/docs/sdd.html

gen_html_docs: gen_mod_sdds gen_dep_sdd
	@$(PYTHON_BIN) $(BUILD_ROOT)/mk/bin/gen_deployment_doc.py $(DEPLOYMENT) $($(DEPLOYMENT)_MODULES)
	
ampcs_clean: ampcs_command_clean ampcs_event_clean ampcs_telemetry_clean

ampcs_merge: ampcs_command_merge ampcs_event_merge ampcs_telemetry_merge

ampcs_command_merge:
	@echo "Generating $(DEPLOYMENT) command dictionary"
	$(AMPCS_COMMAND_MERGE) $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/command.xml $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/mission_header.xml $(foreach module,$($(DEPLOYMENT)_MODULES),$(wildcard $(BUILD_ROOT)/$(module)/$(AMPCS_DICT_MODULE_SUBDIR)/*CmdAc.xml))
	
ampcs_command_clean:
	$(RM_DIR) $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/command.xml
	
ampcs_event_merge:
	@echo "Generating $(DEPLOYMENT) event dictionary"
	$(AMPCS_EVENT_MERGE) $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/evr.xml $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/mission_header.xml $(foreach module,$($(DEPLOYMENT)_MODULES),$(wildcard $(BUILD_ROOT)/$(module)/$(AMPCS_DICT_MODULE_SUBDIR)/*EvrAc.xml))

ampcs_event_clean:
	$(RM_DIR) $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/evr.xml
	
ampcs_telemetry_merge:
	@echo "Generating $(DEPLOYMENT) telemetry dictionary"
	$(AMPCS_TELEMETRY_MERGE) $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/channel.xml $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/mission_header.xml $(foreach module,$($(DEPLOYMENT)_MODULES),$(wildcard $(BUILD_ROOT)/$(module)/$(AMPCS_DICT_MODULE_SUBDIR)/*TlmAc.xml))

ampcs_telemetry_clean:
	$(RM_DIR) $(BUILD_ROOT)/$(DEPLOYMENT)/$(AMPCS_DICT_DEPLOYMENT_SUBDIR)/channel.xml
	
