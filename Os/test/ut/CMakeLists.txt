# Generate unit test
# UT_SOURCE_FILES: Sources for unit test
# UT_MODULES: List of modules linked in unit tests

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/OsQueueTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/IntervalTimerTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/OsValidateFileTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/OsTaskTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/OsFileSystemTest.cpp"
)

set(UT_MODULES
  "${FPRIME_CORE_DIR}/Fw/Obj"
  "${FPRIME_CORE_DIR}/Fw/Types"
  "${FPRIME_CORE_DIR}/Os"
  "${FPRIME_CORE_DIR}/Utils/Hash"
)

#add_unit_test("${UT_SOURCE_FILES}" "${UT_MODULES}")
