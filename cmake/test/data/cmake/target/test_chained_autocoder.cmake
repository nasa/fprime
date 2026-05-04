####
# target/test_chained_autocoder.cmake:
#
# This target sets up a test target for checking that chained autocoders work correctly
####
include(autocoder/autocoder)

function(test_chained_autocoder_add_global_target TARGET)
endfunction(test_chained_autocoder_add_global_target)

function(test_chained_autocoder_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
endfunction(test_chained_autocoder_add_deployment_target)

function(test_chained_autocoder_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    # Run both autocoders in sequence: target autocoder first, then chained autocoder
    run_ac_set("${MODULE}" "autocoder/test_target_autocoder" "autocoder/test_chained_autocoder")

    # Use the variable from this run as set by the autocoders
    add_custom_target("${MODULE}_test_chained_autocode" DEPENDS "${AUTOCODER_GENERATED_OTHER}")

    # Avoid an intermittent parallel-build race with the sibling test_autocoder target
    # (target/test_autocoder.cmake).
    #
    # Both the "test_autocoder" target (registered above us in TestDeployment/CMakeLists.txt)
    # and this "test_chained_autocoder" target run the SAME autocoder, "test_target_autocoder",
    # against this module's .test-target.txt inputs. That autocoder calls
    # add_custom_command(OUTPUT <basename>.test-target.generated.txt COMMAND ${CMAKE_COMMAND} -E copy ...)
    # to produce its intermediate output file. Because both targets depend on that same output,
    # CMake's Makefile generator emits the SAME `cmake -E copy` rule into BOTH submakefiles:
    #   - ${MODULE}_test_autocode.dir/build.make
    #   - ${MODULE}_test_chained_autocode.dir/build.make
    # With `make -jN`, the top-level Makefile2 dispatches both submakes in parallel, both fire
    # their copy rule against the same destination file at once, and `cmake -E copy` racily
    # fails with: `Error copying file "<src>" to "<dst>"`.
    #
    # Forcing _test_chained_autocode to depend on _test_autocode serializes the two: make
    # finishes the copy via _test_autocode first, then when _test_chained_autocode's submake
    # evaluates the (duplicate) rule the destination is already up-to-date and the rule is
    # skipped. The downstream chained-autocoder rule (.chained.txt) still runs normally.
    #
    # The if(TARGET ...) guard is purely defensive: it keeps this target self-contained if
    # `target/test_autocoder` is ever reordered or removed from the registration list (without
    # the guard, add_dependencies() would hard-error in that case).
    if (TARGET "${MODULE}_test_autocode")
        add_dependencies("${MODULE}_test_chained_autocode" "${MODULE}_test_autocode")
    endif()
    add_dependencies("${MODULE}" "${MODULE}_test_chained_autocode")
endfunction(test_chained_autocoder_add_module_target)
