# Makefile to run global make.

#PARSER_VERBOSE =
#export PARSER_VERBOSE

include $(BUILD_ROOT)/mk/configs/host/compute_host.mk
include $(BUILD_ROOT)/mk/configs/host/compute_build_site.mk
include $(BUILD_ROOT)/mk/configs/host/$(HOST)-$(BUILD_SITE).mk
include $(BUILD_ROOT)/mk/configs/builds/builds.mk

# code generation targets

all: $(NATIVE_BUILD)

clean: $(NATIVE_BUILD)_clean

bin_clean: $(NATIVE_BUILD)_bin_clean

rebuild: gen_make clean all dict_install

dictionary: ac_lvl4 dict_install

run: $(NATIVE_BUILD)_run

ac_lvl1:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) AC_LVL1
	
ac_lvl2:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) AC_LVL1 AC_LVL2

ac_lvl3:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) AC_LVL1 AC_LVL2 AC_LVL3
	
ac_lvl4:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) AC_LVL1 AC_LVL2 AC_LVL3 AC_LVL4
	
dictionary: ac_lvl4 dict_install
	
sloc: $(NATIVE_BUILD)_sloc

sloc_report: $(NATIVE_BUILD)_sloc_report

sloc_csv: $(NATIVE_BUILD)_sloc_csv

sloc_csv_report: $(NATIVE_BUILD)_sloc_csv_report

dox: $(NATIVE_BUILD)_dox

clean_dox: $(NATIVE_BUILD)_clean_dox

$(BUILDS): ac_lvl4
	@$(TIME) $(MAKE) $(JOBS) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$@ bin

$(foreach build,$(BUILDS),$(build)_debug): ac_lvl4
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-debug DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _debug,,$@) bin

$(foreach build,$(BUILDS),$(build)_debug_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-debug DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _debug_clean,,$@) clean

$(foreach build,$(BUILDS),$(build)_opt): ac_lvl4
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-opt DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _opt,,$@) bin

$(foreach build,$(BUILDS),$(build)_opt_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-opt DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _opt_clean,,$@) clean

$(foreach build,$(BUILDS),$(build)_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _clean,,$@) clean

$(foreach build,$(BUILDS),$(build)_bin_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _bin_clean,,$@) bin_clean

$(foreach build,$(BUILDS),$(build)_opt_bin_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _opt_bin_clean,,$@) bin_clean

$(foreach build,$(BUILDS),$(build)_debug_bin_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-debug DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _debug_clean,,$@) bin_clean

#$(foreach build,$(BUILDS),$(build)_dox):
#	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(subst _dox,,$@) dox

$(foreach build,$(BUILDS),$(build)_dox):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) gen_dox
	
#$(foreach build,$(BUILDS),$(build)_clean_dox):
#	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(subst _clean_dox,,$@) clean_dox

$(foreach build,$(BUILDS),$(build)_clean_dox):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) clean_dox


$(foreach build,$(BUILDS),$(build)_run):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _run,,$@) run
	
$(foreach build,$(BUILDS),$(build)_sloc):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _sloc,,$@) sloc

$(foreach build,$(BUILDS),$(build)_sloc_report):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _sloc_report,,$@) sloc_report

$(foreach build,$(BUILDS),$(build)_sloc_csv):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _sloc_csv,,$@) sloc_csv

$(foreach build,$(BUILDS),$(build)_sloc_csv_report):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) BUILD=$(subst _sloc_csv_report,,$@) sloc_csv_report

gen_make: compile-templates
	@$(TIME) $(PYTHON_BIN) ${BUILD_ROOT}/mk/src/genmake.py ${BUILD_ROOT}/mk/makefiles

printenv:
	printenv

help: print_builds
	@$(CAT) $(BUILD_ROOT)/mk/docs/mk_help.txt $(BUILD_ROOT)/mk/docs/mk_help_dep.txt
	
print_builds:
	@echo "<Build> targets available: $(BUILDS)"
	
# Ground system targets

dict_clean:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) dict_clean

command_install:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) command_install
	
telemetry_install:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) telemetry_install

events_install:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) events_install

parameters_install:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) parameters_install

serializable_install:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) serializable_install

dict_install: dict_clean
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) dict_install
	
gen_html_docs:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) gen_html_docs
	
sdd:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) gen_dep_sdd
	
sdd_clean:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) dep_sdd_clean

comp_report_gen:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) comp_report_gen
	
ampcs_event_merge:
	$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) ampcs_event_merge
	
ampcs_command_merge:
	$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) ampcs_command_merge
		
ampcs_telemetry_merge:
	$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) ampcs_telemetry_merge

ampcs_clean:
	$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) ampcs_clean
	
ampcs_merge:
	$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile DEPLOYMENT=$(DEPLOYMENT) ampcs_merge
	
run_unit_tests:
	@cd $(BUILD_ROOT)/ptf && $(BUILD_ROOT)/ptf/runtests.sh -i $(USER) -r suites/fw_unit_tests.suite
	
submod:
	git submodule update --init --recursive
	
coverity: coverity_clean
	@$(COVERITY_BIN)/cov-build --dir .coverity_output make COV_BUILD
	@$(COVERITY_BIN)/cov-analyze --user-model-file $(BUILD_ROOT)/mk/coverity/ModelAssert.xmldb --dir .coverity_output
	@$(COVERITY_BIN)/cov-format-errors --html-output .coverity_output/html -dir .coverity_output -x -X
	@echo "HTML output in: .coverity_output/html/index.html"
	
coverity_clean:
	$(RM_DIR) ./.coverity_output
	
include $(BUILD_ROOT)/mk/makefiles/templates.mk
