# Generate unit test
# UT_SOURCE_FILES: Sources for unit test
# UT_MODULES: List of modules linked in unit tests

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Test.cpp"
)

set(UT_MODULES
  "${FPRIME_CORE_DIR}/Fw/SerializableFile"
  "${FPRIME_CORE_DIR}/Fw/SerializableFile/test/TestSerializable"
  "${FPRIME_CORE_DIR}/Fw/Types"
  "${FPRIME_CORE_DIR}/Utils/Hash"
  "${FPRIME_CORE_DIR}/Os"
)

#add_unit_test("${UT_SOURCE_FILES}" "${UT_MODULES}")
