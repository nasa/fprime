###
# Function `cheetah`:
#
# This function sets up the ability to call cheetah to compile cheetah templates and prepare them for use
# within the auto-coder. Here we setup the "compilation" of Cheetah templates.
#
# - **CHEETAH_TEMPLATES:** list of cheetah templates
##
function(cheetah CHEETAH_TEMPLATES)
    # Derive output file names, which will be added as dependencies for the code generation step. This
    # forces cheetah templates to be compiled and up-to-date before running the auto-coder.
    string(REPLACE ".tmpl" ".py" PYTHON_TEMPLATES "${CHEETAH_TEMPLATES}")
    # Setup the cheetah-compile command that runs the physical compile of the above statement. This
    # controls the work to be done to create PYTHON_TEMPLATES from ${CHEETAH_TEMPLATES.
    find_program(CHEETAH_EXE NAMES "cheetah-compile" "cheetah-compile3")
    add_custom_command(
            OUTPUT ${PYTHON_TEMPLATES}
            COMMAND ${CHEETAH_EXE} ${CHEETAH_TEMPLATES}
            DEPENDS ${CHEETAH_TEMPLATES}
    )
    # Add the above PYTHON_TEMPLATES to the list of sources for the CODEGEN_TARGET target. Thus they will be
    # built as dependencies for the CODEGEN step.
    set_property(GLOBAL APPEND PROPERTY CODEGEN_OUTPUTS ${PYTHON_TEMPLATES})
endfunction(cheetah)


function(setup_ai_autocode_variant VARIANT_ARGS OUTPUT_DIR EXTRA_COMMANDS AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES)
    find_program(PYTHON NAMES python3 python)
    string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
    add_custom_command(
            OUTPUT  ${GENERATED_FILES}
            BYPRODUCTS ${GENERATED_FILES}
            COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils
            BUILD_ROOT=${FPRIME_BUILD_LOCATIONS_SEP}:${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/F-Prime
            FPRIME_AC_CONSTANTS_FILE=${FPRIME_AC_CONSTANTS_FILE}
            PYTHON_AUTOCODER_DIR=${PYTHON_AUTOCODER_DIR}
            ${PYTHON} ${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/bin/codegen.py -p ${OUTPUT_DIR} --build_root ${VARIANT_ARGS} ${AC_INPUT_FILE}
            COMMAND ${EXTRA_COMMANDS}
            DEPENDS ${AC_INPUT_FILE} ${FILE_DEPENDENCIES} ${FPRIME_AC_CONSTANTS_FILE} ${MODULE_DEPENDENCIES} ${CODEGEN_TARGET}
    )
endfunction(setup_ai_autocode_variant)
