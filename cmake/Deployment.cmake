# Contains functions for building deployments

function(generate_deployement DEPLOYMENT_NAME AUTOCODER_INPUT_FILES SOURCE_FILES DEPLOYMENT_MODULES)

  message(STATUS "Adding executable: ${DEPLOYMENT_NAME}")
  # add_executable(<name> [sources...])
  #TODO: ramification of these changes
  #TODO: custom executable
  add_executable("${DEPLOYMENT_NAME}" "${SOURCE_FILES}")

  # Generate module from sources
  generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

  # Generate module library names
  set(LIBRARIES "")
  foreach(MODULE_PATH ${DEPLOYMENT_MODULES})
    # Sets MODULE_NAME to unique name based on path
    get_module_name(${MODULE_PATH})
    list(APPEND LIBRARIES ${MODULE_NAME})
  endforeach()

  # Sets MODULE_NAME to unique name based on path
  get_module_name(${CMAKE_CURRENT_LIST_DIR})
  target_link_libraries("${DEPLOYMENT_NAME}" ${MODULE_NAME} "${LIBRARIES}")

  # Include Framework configuration target
  target_link_libraries("${DEPLOYMENT_NAME}" "Fw_Cfg")

  # TODO Thread library per target
  target_link_libraries("${DEPLOYMENT_NAME}" "${CMAKE_THREAD_LIBS_INIT}")

endfunction(generate_deployement)
