
compile-templates:
	@echo "Generating templates"
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/component && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/test && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/port && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/serialize && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/commands && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/events && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/channels && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/html && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/md && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/gtest && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/impl && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/topology && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/test_impl && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/enums && $(MAKE) -f Makefile

clean-templates:
	@echo "Cleaning templates"
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/component && $(MAKE) -f Makefile  clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/test && $(MAKE) -f Makefile  clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/port && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/serialize && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/commands && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/events && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/channels && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/html && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/md && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/gtest && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/impl && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/topology && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/test_impl && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/generators/templates/enums && $(MAKE) -f Makefile clean
	
