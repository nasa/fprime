# Generate unit test
# UT_SOURCE_FILES: Sources for unit test
# UT_MODULES: List of modules linked in unit tests

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
)

set(UT_MODULES
  "${FPRIME_CORE_DIR}/Fw/FilePacket/GTest"
  "${FPRIME_CORE_DIR}/Fw/FilePacket"
  "${FPRIME_CORE_DIR}/Fw/Buffer"
  "${FPRIME_CORE_DIR}/Fw/Types"
  "${FPRIME_CORE_DIR}/Fw/Types/GTest"
  "${FPRIME_CORE_DIR}/CFDP/Checksum/GTest"
  "${FPRIME_CORE_DIR}/CFDP/Checksum"
)

#add_unit_test("${UT_SOURCE_FILES}" "${UT_MODULES}")
