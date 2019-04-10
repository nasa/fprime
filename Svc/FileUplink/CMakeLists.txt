####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FileUplinkComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/FileUplink.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Warnings.cpp"
)
set(MOD_DEPS
  Os
  Fw/Buffer
  Fw/FilePacket
  Utils/Hash
  CFDP/Checksum
)
register_fprime_module()

##TODO-MSTARCH add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")

### UTs ###
##TODO-MSTARCH: set(UT_SOURCES
##TODO-MSTARCH: 	${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp
##TODO-MSTARCH: 	${CMAKE_CURRENT_LIST_DIR}/test/ut/Main.cpp
##TODO-MSTARCH: )
##TODO-MSTARCH: add_unit_test("${UT_SOURCES}" "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")
