####
# Unit_Test.cmake:
#
# Testing does not properly handle unit test dependencies in some versions of CMake. Therefore,
# we follow a standard workaround from CMake users and create a "check" target used to run the
# tests while rolling-up the dependencies properly. Thus tests may be run using `make check` as
# opposed to the standard CMake call. The CMake test support functions are still used.
#
####
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

####
# Function `unit_test_component_autocoder`:
#
# Performs registration of the autocoder step for the generation of GTestBase.hpp, GTestBase.cpp,
# TesterBase.cpp, and TesterBase.hpp. These autocoding steps automate, and keep up-to-date the
# above files saving time.
# 
# - **EXE_NAME:** name of exe (unit test exe)
# - **SOURCE_FILES:** source files to provide for autocoding
####
function(unit_test_component_autocoder EXE_NAME SOURCE_FILES)
  # Search for component xml files
  foreach(TEST_SOURCE ${SOURCE_FILES})
    string(REGEX MATCH "([./a-zA-Z0-9\-_]+)ComponentAi.xml" COMPONENT_XML ${TEST_SOURCE})
    if(NOT ${COMPONENT_XML} STREQUAL "")
      # Extract component name
      string(REGEX REPLACE "([a-zA-Z0-9\-_]+)(ComponentAi.xml)" "\\1" COMPONENT_NAME ${COMPONENT_XML})
      get_filename_component(RAW_XML ${COMPONENT_XML} NAME)

      # Invoke autocoder to produce unit test base classes
      if (GENERATE_AC_IN_SOURCE)
          set(AUTOCODE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Autocode")
      else()
          set(AUTOCODE_DIR "${CMAKE_CURRENT_BINARY_DIR}/Autocode")
      endif()
      set(GTEST_SOURCE "${AUTOCODE_DIR}/GTestBase.cpp")
      set(BASE_SOURCE "${AUTOCODE_DIR}/TesterBase.cpp")
      set(GTEST_HEADER "${AUTOCODE_DIR}/GTestBase.hpp")
      set(BASE_HEADER "${AUTOCODE_DIR}/TesterBase.hpp")
      target_include_directories(${EXE_NAME} PUBLIC ${AUTOCODE_DIR})
      add_custom_command(
        OUTPUT ${GTEST_SOURCE} ${BASE_SOURCE} ${GTEST_HEADER} ${BASE_HEADER}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${AUTOCODE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${TEST_SOURCE} ${AUTOCODE_DIR}
        COMMAND ${CMAKE_COMMAND} -E chdir ${AUTOCODE_DIR} ${CMAKE_COMMAND} -E env pwd
        COMMAND ${CMAKE_COMMAND} -E chdir ${AUTOCODE_DIR}
        ${CMAKE_COMMAND} -E env PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils BUILD_ROOT=${FPRIME_CURRENT_BUILD_ROOT}
        ${PYTHON_AUTOCODER_DIR}/bin/codegen.py -p ${AUTOCODE_DIR} --build_root ${RAW_XML}
        COMMAND ${CMAKE_COMMAND} -E chdir ${AUTOCODE_DIR}
        ${CMAKE_COMMAND} -E env PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils BUILD_ROOT=${FPRIME_CURRENT_BUILD_ROOT}
        ${PYTHON_AUTOCODER_DIR}/bin/codegen.py -p ${AUTOCODE_DIR} --build_root -u ${RAW_XML}
        COMMAND ${CMAKE_COMMAND} -E remove ${AUTOCODE_DIR}/Tester.hpp ${AUTOCODE_DIR}/Tester.cpp
        COMMAND ${CMAKE_COMMAND} -E echo "All done Yo!"
        DEPENDS ${TEST_SOURCE}
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


####
# Function `generate_ut`:
#
# Generates the actual unit test, dependencies, and call the autocoder.
#
# - **UT_EXE_NAME:** name of the UT executable to be created
# - **UT_SOURCES_INPUT:** sources to split into source and autocoder file
# - **MOD_DEPS_INPUT:** dependencies split into thread and module dependencies
####
function(generate_ut UT_EXE_NAME UT_SOURCES_INPUT MOD_DEPS_INPUT)
    # Set the following variables from the existing SOURCE_FILES and LINK_DEPS by splitting them into
    # their separate peices. 
    #
    # AUTOCODER_INPUT_FILES = *.xml and *.txt in SOURCE_FILES_INPUT, fed to auto-coder
    # SOURCE_FILES = all other items in SOURCE_FILES_INPUT, set as compile-time sources
    # LINK_DEPS = -l link flags given to DEPS_INPUT
    # MOD_DEPS = All other module inputs DEPS_INPUT
    split_source_files("${UT_SOURCES_INPUT}")
    split_dependencies("${MOD_DEPS_INPUT}")
    generate_executable(${UT_EXE_NAME} "${SOURCE_FILES}" "${MOD_DEPS_INPUT}")
    # Generate the UTs w/ autocoding and add the other sources  
    unit_test_component_autocoder(${UT_EXE_NAME} "${AUTOCODER_INPUT_FILES}")
    # Link modules
    target_link_libraries(
        "${UT_EXE_NAME}"
        "${GTEST_TARGET}"
        "-lpthread" #TODO: fix this
    )
    # Add test and dependencies to the "check" target
    add_test(NAME ${UT_EXE_NAME} COMMAND ${UT_EXE_NAME})
    add_dependencies(check ${UT_EXE_NAME})
    # Link library list output on per-module basis
    if (CMAKE_DEBUG_OUTPUT)
	    print_dependencies(${UT_EXE_NAME})
    endif()
endfunction()