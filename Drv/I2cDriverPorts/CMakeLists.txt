####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/I2cPortAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/I2cWriteReadPortAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/I2cStatusEnumAi.xml"
)

register_fprime_module()
