####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SignalTypeEnumAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SignalSetArrayAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SignalPairSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SignalPairSetArrayAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SignalInfoSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SignalGenComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SignalGen.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SignalGenComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
register_fprime_ut()
