# Generate unit test
# UT_SOURCE_FILES: Sources for unit test
# UT_MODULES: List of modules linked in unit tests

set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/CmdDispatcher/CommandDispatcherComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImplTester.cpp"
)

set(UT_MODULES
  "${FPRIME_CORE_DIR}/Svc/CmdDispatcher"
  "${FPRIME_CORE_DIR}/Fw/Tlm"
  "${FPRIME_CORE_DIR}/Fw/Comp"
  "${FPRIME_CORE_DIR}/Fw/Cmd"
  "${FPRIME_CORE_DIR}/Fw/Log"
  "${FPRIME_CORE_DIR}/Fw/Obj"
  "${FPRIME_CORE_DIR}/Fw/Port"
  "${FPRIME_CORE_DIR}/Fw/Time"
  "${FPRIME_CORE_DIR}/Fw/Types"
  "${FPRIME_CORE_DIR}/Fw/Com"
  "${FPRIME_CORE_DIR}/Svc/Sched"
  "${FPRIME_CORE_DIR}/Svc/Ping"
  "${FPRIME_CORE_DIR}/Os"
)

#add_unit_test("${UT_SOURCE_FILES}" "${UT_MODULES}")
