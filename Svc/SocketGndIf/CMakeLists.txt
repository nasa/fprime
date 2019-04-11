####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SvcSocketGndIfImpl.cpp"
)
set(MOD_DEPS
    Svc/GndIf
    Fw/Buffer
)
register_fprime_module()
