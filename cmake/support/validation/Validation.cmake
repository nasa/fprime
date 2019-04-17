####
# Validation.cmake:
#
# Allows for validation hooks in the F prime cmake system.
####

####
# Function `run_setup_validation`:
#
# Runs validation command during the CMake setup period. This runs as soon as this function is
# called, and returns the results of to the VALIDATION_RESULT variable in parent scope.
#
####
function(run_setup_validation)
    message(STATUS "[VALIDATION] Running: ${ARGN}")
    # Run the process and check the result
    execute_process(
        COMMAND ${PROCESS_EXE} ${ARGN}
        RESULT_VARIABLE ERR_RETURN
    )
    if (${ERR_RETURN})
        set(VALIDATION_RESULT FALSE PARENT_SCOPE)
        return()
    endif()
    set(VALIDATION_RESULT TRUE PARENT_SCOPE)
    return()
endfunction(run_setup_validation)

####
# Function `setup_validation_command`:
#
# Setup validation command to run in the system.
####
function(setup_validation_command VALIDATION_TIME)
    if (VALIDATION_TIME STREQUAL "SETUP")
    	run_setup_validation(${PROCESS_EXE} ${ARGN})
    	if (NOT VALIDATION_RESULT)
    	    message(FATAL_ERROR "[VALIDATION] Validation failed for: ${ARGN}")
    	endif()
    elseif(VALIDATION_TIME STREQUAL "PRE-AUTOCODE")
    
    elseif(VALIDATION_TIME STREQUAL "PRE-BUILD")
    
    elseif(VALIDATION_TIME STREQUAL "POST-BUILD")
    
    else()
        message(FATAL_ERROR "[VALIDATION] Invalid validation time '${VALIDATION_TIME}'") 
    endif()
endfunction(setup_validation_command)

# Registers the pipsetup check
setup_validation_command("SETUP" "python" "${CMAKE_CURRENT_LIST_DIR}/pipsetup.py")
