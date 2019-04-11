# Generate unit test
# UT_SOURCE_FILES: Sources for unit test
# UT_MODULES: List of modules linked in unit tests

set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/ComLogger/ComLoggerComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
)

set(UT_MODULES
  "${FPRIME_CORE_DIR}/Svc/ComLogger"
  "${FPRIME_CORE_DIR}/Svc/Ping"
  "${FPRIME_CORE_DIR}/Fw/Cmd"
  "${FPRIME_CORE_DIR}/Fw/Comp"
  "${FPRIME_CORE_DIR}/Fw/Port"
  "${FPRIME_CORE_DIR}/Fw/Prm"
  "${FPRIME_CORE_DIR}/Fw/Time"
  "${FPRIME_CORE_DIR}/Fw/Tlm"
  "${FPRIME_CORE_DIR}/Fw/Types"
  "${FPRIME_CORE_DIR}/Fw/Log"
  "${FPRIME_CORE_DIR}/Fw/Obj"
  "${FPRIME_CORE_DIR}/Fw/Com"
  "${FPRIME_CORE_DIR}/Os"
  "${FPRIME_CORE_DIR}/Utils/Hash"
)

#add_unit_test("${UT_SOURCE_FILES}" "${UT_MODULES}")

# Add test outputs to clean target
file(GLOB TEST_OUTPUTS "*.com*")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${TEST_OUTPUTS}")
