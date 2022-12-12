####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ConfigCheck.cpp"
)
register_fprime_module()
if (FPRIME_USE_BAREMETAL_SCHEDULER)
    target_compile_definitions(Fw_Cfg PUBLIC FW_BAREMETAL_SCHEDULER=1)
else()
    target_compile_definitions(Fw_Cfg PUBLIC FW_BAREMETAL_SCHEDULER=0)
endif()
