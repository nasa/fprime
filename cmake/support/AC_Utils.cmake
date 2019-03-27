###
# Cheetah Templates:
#
# This function sets up the ability to call cheetah to compile chetah templates and prepare them for use
# within the auto-coder. Here we setup the 
#
# TODO: when/if the autocoder stops depending on generated templates, this function may be removed. Otherwise,
# this must be updated to remove cheetah dependence for whatever new templating engine is used.
##
function(cheetah CHEETAH_TEMPLATES)
  # Derive output file names, which will be added as dependencies for the code generation step. This
  # forces cheetah templates to be compiled and up-to-date before running the auto-coder.
  string(REPLACE ".tmpl" ".py" PYTHON_TEMPLATES "${CHEETAH_TEMPLATES}")
  # Setup the cheetah-compile command that runs the physical compile of the above statement. This
  # controls the work to be done to create PYTHON_TEMPLATES from ${CHEETAH_TEMPLATES. 
  add_custom_command(
    OUTPUT ${PYTHON_TEMPLATES}
    COMMAND cheetah-compile ${CHEETAH_TEMPLATES}
    DEPENDS ${CHEETAH_TEMPLATES}
  )
  # Add the above PYTHON_TEMPLATES to the list of sources for the CODEGEN_TARGET target. Thus they will be
  # built as dependencies for the CODEGEN step.
  target_sources(
    ${CODEGEN_TARGET}
    PRIVATE ${PYTHON_TEMPLATES}
  )
endfunction(cheetah)
