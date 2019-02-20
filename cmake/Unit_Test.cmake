# Setup testing infrastructure
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

# Create a unit test library
function(generate_ut_library MODULE_NAME SOURCE_FILES)
  # Generate unit test module name
  string(CONCAT UT_MODULE_NAME ${MODULE_NAME} "_ut")

  add_library(
    ${UT_MODULE_NAME}
    STATIC
    EXCLUDE_FROM_ALL # Do not include unit test builds in all
    ${SOURCE_FILES}
  )

  # Add unit test compile definitions
  target_compile_definitions(
    ${UT_MODULE_NAME}
    PRIVATE
    "-DBUILD_UT"
    "-DPROTECTED=public"
    "-DPRIVATE=public"
  )

  # Add dependency on original module
  add_dependencies(${UT_MODULE_NAME} ${MODULE_NAME})

endfunction(generate_ut_library)

# Invoke autocoder to generate unit test files
function(unit_test_component_autocoder EXE_NAME SOURCE_FILES)
  # Search for component xml files
  foreach(TEST_SOURCE ${SOURCE_FILES})
    string(REGEX MATCH "([./a-zA-Z0-9\-_]+)ComponentAi.xml" COMPONENT_XML ${TEST_SOURCE})
    if(NOT ${COMPONENT_XML} STREQUAL "")
      # Extract component name
      string(REGEX REPLACE "([a-zA-Z0-9\-_]+)(ComponentAi.xml)" "\\1" COMPONENT_NAME ${COMPONENT_XML})
      get_filename_component(RAW_XML ${COMPONENT_XML} NAME)
      get_filename_component(COMPONENT_DIR ${COMPONENT_XML} DIRECTORY)

      # Invoke autocoder to produce unit test base classes
      set(AUTOCODE_DIR "${CMAKE_CURRENT_LIST_DIR}/Autocode")
      set(GTEST_SOURCE "${AUTOCODE_DIR}/GTestBase.cpp")
      set(BASE_SOURCE "${AUTOCODE_DIR}/TesterBase.cpp")
      set(GTEST_HEADER "${AUTOCODE_DIR}/GTestBase.hpp")
      set(BASE_HEADER "${AUTOCODE_DIR}/TesterBase.hpp")
      add_custom_command(
        OUTPUT ${GTEST_SOURCE} ${BASE_SOURCE} ${GTEST_HEADER} ${BASE_HEADER}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${AUTOCODE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${COMPONENT_XML} ${AUTOCODE_DIR}
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils BUILD_ROOT=${FPRIME_CURRENT_BUILD_ROOT}
        ${PYTHON_AUTOCODER_DIR}/bin/codegen.py -p ${AUTOCODE_DIR} --build_root ${RAW_XML}
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils BUILD_ROOT=${FPRIME_CURRENT_BUILD_ROOT}
        ${PYTHON_AUTOCODER_DIR}/bin/codegen.py -p ${AUTOCODE_DIR} --build_root -u ${RAW_XML}
        DEPENDS ${COMPONENT_XML}
      )

      # Add autocode sources to module
      target_sources(
        ${EXE_NAME}
        PRIVATE
        ${GTEST_SOURCE}
        ${BASE_SOURCE}
      )

    endif()
  endforeach()
endfunction(unit_test_component_autocoder)

# Add a unit test
function(add_unit_test SOURCE_FILES MODULES_LIST)
  # Sets MODULE_NAME to unique name based on path
  set(MODULE_NAME "")
  get_module_name(${CMAKE_CURRENT_LIST_DIR})
  set(UT_EXE_NAME ${MODULE_NAME})
  message(STATUS "\tAdding unit test: ${UT_EXE_NAME}")

  # Create excutable
  add_executable(
    ${UT_EXE_NAME}
    EXCLUDE_FROM_ALL
    ${SOURCE_FILES}
  )

  # Invoke componet autocoder for unit tests
  unit_test_component_autocoder(${UT_EXE_NAME} ${SOURCE_FILES})

  # Generate ut libary names
  set(UT_LIBRARIES "")
  foreach(UT_MODULE_PATH ${MODULES_LIST})
    # Sets MODULE_NAME to unique name based on path
    get_module_name(${UT_MODULE_PATH})
    string(CONCAT UT_MODULE_NAME ${MODULE_NAME} "_ut")
    list(APPEND UT_LIBRARIES ${UT_MODULE_NAME})
  endforeach()

  # Link modules
  target_link_libraries(
    "${UT_EXE_NAME}"
    "${GTEST_TARGET}"
    "${CMAKE_THREAD_LIBS_INIT}"
    "${UT_LIBRARIES}"
  )

  # Add unit tests definitions
  target_compile_definitions(
    ${UT_EXE_NAME}
    PRIVATE
    "-DBUILD_UT"
    "-DPROTECTED=public"
    "-DPRIVATE=public"
  )

  # Add test
  add_test(
    NAME ${UT_EXE_NAME}
    COMMAND ${UT_EXE_NAME}
  )

  # Add dependency on check
  add_dependencies(
    check
    ${UT_EXE_NAME}
  )

endfunction(add_unit_test)
