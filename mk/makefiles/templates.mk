
compile-templates:
	@echo "Generating templates"
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/component && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/test && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/port && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/serialize && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/commands && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/events && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/channels && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/html && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/md && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/gtest && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/impl && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/topology && $(MAKE) -f Makefile
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/test_impl && $(MAKE) -f Makefile

clean-templates:
	@echo "Cleaning templates"
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/component && $(MAKE) -f Makefile  clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/test && $(MAKE) -f Makefile  clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/port && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/serialize && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/commands && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/events && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/channels && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/html && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/md && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/gtest && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/impl && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/topology && $(MAKE) -f Makefile clean
	@cd $(BUILD_ROOT)/Autocoders/src/generators/templates/test_impl && $(MAKE) -f Makefile clean
	
