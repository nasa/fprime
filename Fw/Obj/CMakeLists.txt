####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ObjBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SimpleObjRegistry.cpp"
)
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
)
if (BUILD_SHARED_LIBS)
  list(APPEND MOD_DEPS "Fw/Logger")
endif()

register_fprime_module()
