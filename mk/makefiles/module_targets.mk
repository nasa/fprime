include $(BUILD_ROOT)/mk/configs/host/compute_host.mk
include $(BUILD_ROOT)/mk/configs/host/compute_build_site.mk
include $(BUILD_ROOT)/mk/configs/host/$(HOST)-$(BUILD_SITE).mk
include $(BUILD_ROOT)/mk/configs/builds/builds.mk

all: $(NATIVE_BUILD)

clean: $(NATIVE_BUILD)_clean

bin_clean: $(NATIVE_BUILD)_bin_clean

rebuild: gen_make clean all

ac_clean: $(NATIVE_BUILD)_ac_clean

bin_clean: $(NATIVE_BUILD)_bin_clean

ut: ut_$(NATIVE_BUILD)

ut_clean: ut_clean_$(NATIVE_BUILD)

run_ut: run_ut_$(NATIVE_BUILD)

ut_nocov: ut_nocov_$(NATIVE_BUILD)

ut_nocov_clean: ut_nocov_clean_$(NATIVE_BUILD)

run_ut_nocov: run_ut_nocov_$(NATIVE_BUILD)

integ: integ_$(NATIVE_BUILD)

integ_clean: integ_clean_$(NATIVE_BUILD)

run_integ: run_integ_$(NATIVE_BUILD)

cov: cov_$(NATIVE_BUILD)

sloc: sloc_$(NATIVE_BUILD)


ac_lvl1:
	@echo "Building module $(MODULE) code generation (level 1)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile MOD_TARGET=$(MODULE) MOD_AC_LVL1

ac_lvl2:
	@echo "Building module $(MODULE) code generation (level 2)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile MOD_TARGET=$(MODULE) MOD_AC_LVL1 MOD_AC_LVL2

ac_lvl3:
	@echo "Building module $(MODULE) code generation (level 3)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile MOD_TARGET=$(MODULE) MOD_AC_LVL1 MOD_AC_LVL2 MOD_AC_LVL3 

ac_lvl4:
	@echo "Building module $(MODULE) code generation (level 4)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile MOD_TARGET=$(MODULE) MOD_AC_LVL1 MOD_AC_LVL2 MOD_AC_LVL3 MOD_AC_LVL4 

$(BUILDS): ac_lvl4
	@echo "Building module $(MODULE) for $@"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$@ $(MODULE)

$(foreach build,$(BUILDS),$(build)_opt): ac_lvl4
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-opt BUILD=$(subst _opt,,$@) $(MODULE)

$(foreach build,$(BUILDS),$(build)_optclean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-opt BUILD=$(subst _opt_clean,,$@) $(MODULE)_clean

$(foreach build,$(BUILDS),$(build)_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(subst _clean,,$@) $(MODULE)_clean

$(foreach build,$(BUILDS),$(build)_ac_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(subst _ac_clean,,$@) $(MODULE)_ac_clean

$(foreach build,$(BUILDS),$(build)_bin_clean):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(subst _bin_clean,,$@) $(MODULE)_bin_clean

testcomp:
	@echo "Generating test components for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile $(MODULE)_testcomp

impl:
	@echo "Generating implementation template for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile $(MODULE)_impl

$(foreach build,$(BUILDS),ut_$(build)):
	@echo "Compiling $@ unit test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-ut BUILD=$(subst ut_,,$@) test_$(MODULE)testut

$(foreach build,$(BUILDS),ut_nocov_$(build)):
	@echo "Compiling $@ unit test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-ut-nocov BUILD=$(subst ut_nocov_,,$@) test_$(MODULE)testut

$(foreach build,$(BUILDS),ut_clean_$(build)):
	@echo "Cleaning unit test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-ut BUILD=$(subst ut_clean_,,$@) test_$(MODULE)testut_clean

$(foreach build,$(BUILDS),ut_nocov_clean_$(build)):
	@echo "Cleaning unit test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-ut-nocov BUILD=$(subst ut_nocov_clean_,,$@) test_$(MODULE)testut_clean

$(foreach build,$(BUILDS),run_ut_$(build)):
	@echo "Running unit test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-ut BUILD=$(subst run_ut_,,$@) run_ut

$(foreach build,$(BUILDS),run_ut_nocov_$(build)):
	@echo "Running unit test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-ut-nocov BUILD=$(subst run_ut_nocov_,,$@) run_ut

$(foreach build,$(BUILDS),integ_$(build)):
	@echo "Compiling $@ integration test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-integ BUILD=$(subst integ_,,$@) test_$(MODULE)testinteg

$(foreach build,$(BUILDS),integ_clean_$(build)):
	@echo "Cleaning integration test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-integ BUILD=$(subst integ_clean_,,$@) test_$(MODULE)testinteg_clean

$(foreach build,$(BUILDS),run_integ_$(build)):
	@echo "Running integration test for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-integ BUILD=$(subst run_integ_,,$@) run_integ

$(foreach build,$(BUILDS),cov_$(build)):
	@echo "Checking coverage for $(MODULE)"
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile COMP=comp-ut BUILD=$(subst cov_,,$@) MOD_TARGET=$(MODULE) cov

$(foreach build,$(BUILDS),sloc_$(build)):
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(subst sloc_,,$@) sloc_build $(MODULE)_sloc $(MODULE)_sloc_dump

gen_make: compile-templates
	@echo "Regenerating global Makefile"
	@$(TIME) $(PYTHON_BIN) $(BUILD_ROOT)/mk/src/genmake.py ${BUILD_ROOT}/mk/makefiles

gen_dox:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(NATIVE_BUILD) gen_dox_$(MODULE)
	
show_dox:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(NATIVE_BUILD) show_dox_$(MODULE)
	
clean_dox:
	@$(TIME) $(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile BUILD=$(NATIVE_BUILD) clean_dox_$(MODULE)
	
sdd: 
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile $(MODULE)_sdd
	
sdd_clean:
	@$(MAKE) -f $(BUILD_ROOT)/mk/makefiles/Makefile $(MODULE)_sdd_clean
	
help: print_builds
	@$(CAT) $(BUILD_ROOT)/mk/docs/mk_help.txt $(BUILD_ROOT)/mk/docs/mk_help_mod.txt
	
print_builds:
	@echo "<Build> targets available: $(BUILDS)"
	
coverity: coverity_clean
	@$(COVERITY_BIN)/cov-build --dir .coverity_output make bin_clean all
	@$(COVERITY_BIN)/cov-analyze --dir .coverity_output
	@$(COVERITY_BIN)/cov-format-errors --html-output .coverity_output/html -dir .coverity_output -x -X
	@echo "HTML output in: .coverity_output/html/index.html"
	
coverity_clean:
	$(RM_DIR) ./.coverity_output	
	
include $(BUILD_ROOT)/mk/makefiles/templates.mk	
