# Generate unit test
# UT_SOURCE_FILES: Sources for unit test
# UT_MODULES: List of modules linked in unit tests

set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/Time/TimeComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
)

set(UT_MODULES
  "${FPRIME_CORE_DIR}/Svc/LinuxTime"
  "${FPRIME_CORE_DIR}/Svc/Time"
  "${FPRIME_CORE_DIR}/Fw/Time"
  "${FPRIME_CORE_DIR}/Fw/Comp"
  "${FPRIME_CORE_DIR}/Fw/Types"
  "${FPRIME_CORE_DIR}/Fw/Obj"
  "${FPRIME_CORE_DIR}/Fw/Port"
  "${FPRIME_CORE_DIR}/Os"
)

#add_unit_test("${UT_SOURCE_FILES}" "${UT_MODULES}")
