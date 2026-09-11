####
# test_directive_parser.py:
#
# Tests for `fprime__process_module_setup` (cmake/module.cmake), the argument parser behind every
# register_fprime_* call. Each case configures the tiny TestDirectiveParser project once, handing the
# parser an argument list and checking either the INTERNAL_* outputs or the fatal error produced.
#
####
import re
import shutil
import tempfile
from pathlib import Path

import pytest

from . import cmake
from . import settings

SOURCE_DIRECTORY = settings.DATA_DIR / "TestDirectiveParser"
OUTPUT_PATTERN = re.compile(r"\[test\] INTERNAL_(\w+)=(.*)")

# Resolved paths, matching what the parser reports for existing files
STUB_CPP = str((SOURCE_DIRECTORY / "stub.cpp").resolve())
STUB2_CPP = str((SOURCE_DIRECTORY / "stub2.cpp").resolve())
STUB_HPP = str((SOURCE_DIRECTORY / "stub.hpp").resolve())
MODEL_FPP = str((SOURCE_DIRECTORY / "model.fpp").resolve())
STUB_LD = str((SOURCE_DIRECTORY / "stub.ld").resolve())
MISSING = str(SOURCE_DIRECTORY / "does-not-exist.cpp")

# Additional control sets as supplied by the public API (cmake/API.cmake)
LIBRARY_CONTROLS = ["INTERFACE", "OBJECT"]
UT_CONTROLS = [
    "INCLUDE_GTEST",
    "UT_AUTO_HELPERS",
    "CHOOSES_IMPLEMENTATIONS",
    "TESTED_MODULE",
]
CONFIG_CONTROLS = [
    "CONFIGURATION_OVERRIDES",
    "STATIC",
    "INTERFACE",
    "CHOOSES_IMPLEMENTATIONS",
    "GLOBAL_IMPLICIT_DEPENDENCY",
    "BASE_CONFIG",
]
# Base list-valued directives
LIST_DIRECTIVES = [
    "HEADERS",
    "SOURCES",
    "DEPENDS",
    "AUTOCODER_INPUTS",
    "REQUIRES_IMPLEMENTATIONS",
    "LINK_DEPENDS",
]


def other_directive(directive):
    """A populated list directive distinct from the one under test"""
    return ["HEADERS", STUB_HPP] if directive != "HEADERS" else ["SOURCES", STUB_CPP]


def run_parser(
    args, module_type="Library", additional=None, current_module=None, legacy=None
):
    """Configure TestDirectiveParser once and return (return_code, outputs, stderr_text)"""
    options = {"PARSER_ARGS": ";".join(args), "PARSER_MODULE_TYPE": module_type}
    if additional:
        options["PARSER_ADDITIONAL_CONTROL_SETS"] = ";".join(additional)
    if current_module is not None:
        options["PARSER_CURRENT_MODULE"] = current_module
    for name, value in (legacy or {}).items():
        options[f"PARSER_LEGACY_{name}"] = ";".join(value)
    build_directory = Path(tempfile.mkdtemp())
    try:
        return_code, stdout, stderr = cmake.run_cmake(
            SOURCE_DIRECTORY, build_directory, options
        )
    finally:
        shutil.rmtree(build_directory, ignore_errors=True)
    outputs = {}
    for line in stdout:
        match = OUTPUT_PATTERN.search(line)
        if match:
            value = match.group(2).strip()
            outputs[match.group(1)] = value.split(";") if value else []
    return return_code, outputs, "".join(stderr)


def assert_accepted(expected, **kwargs):
    """Parser must succeed and every INTERNAL_* output must match expected exactly"""
    return_code, outputs, stderr = run_parser(**kwargs)
    assert return_code == 0, f"Parser rejected valid arguments:\n{stderr}"
    assert outputs == expected, f"Parser outputs differ: {outputs} != {expected}"


def assert_rejected(message, **kwargs):
    """Parser must fail configuration and report the given message"""
    return_code, _, stderr = run_parser(**kwargs)
    assert return_code != 0, "Parser accepted invalid arguments"
    assert message in stderr, f"Expected '{message}' in:\n{stderr}"


def base_outputs(name="Foo", **lists):
    """Expected outputs for a successful parse: module name, empty add-options, and given lists"""
    expected = {"MODULE_NAME": [name], "CMAKE_ADD_OPTIONS": []}
    expected.update(lists)
    return expected


# ---- Module name resolution ----


def test_explicit_module_name():
    """First non-directive argument names the module"""
    assert_accepted(base_outputs(SOURCES=[STUB_CPP]), args=["Foo", "SOURCES", STUB_CPP])


def test_current_module_fallback():
    """FPRIME_CURRENT_MODULE names the module when no name is supplied"""
    assert_accepted(
        base_outputs("Bar", SOURCES=[STUB_CPP]),
        args=["SOURCES", STUB_CPP],
        current_module="Bar",
    )


def test_current_module_fallback_unit_test():
    """Unit tests derive their name from FPRIME_CURRENT_MODULE and the UT target suffix"""
    assert_accepted(
        base_outputs("Bar_ut_exe", SOURCES=[STUB_CPP]),
        args=["SOURCES", STUB_CPP],
        module_type="Unit Test",
        additional=UT_CONTROLS,
        current_module="Bar",
    )


def test_missing_module_name():
    """No name and no FPRIME_CURRENT_MODULE is a fatal error"""
    assert_rejected("FPRIME_CURRENT_MODULE not defined", args=["SOURCES", STUB_CPP])


def test_module_name_is_not_a_control_word():
    """A leading control word is a directive, not a module name"""
    assert_rejected(
        "FPRIME_CURRENT_MODULE not defined", args=["SOURCES", "SOURCES", STUB_CPP]
    )


# ---- Required arguments ----


def test_no_arguments_module():
    """A module with no directives at all is rejected"""
    assert_rejected("Must supply SOURCES to register_fprime_*", args=["Foo"])


def test_no_arguments_unit_test():
    """A unit test with no directives at all is rejected with the UT message"""
    assert_rejected(
        "Must supply SOURCES to register_fprime_ut",
        args=["Foo"],
        module_type="Unit Test",
        additional=UT_CONTROLS,
    )


def test_value_before_directive():
    """A value before any control word is rejected"""
    assert_rejected("must be specified before list elements", args=["Foo", STUB_CPP])


def test_interface_without_sources():
    """Header-only modules legitimately omit SOURCES (e.g. Fw/Sm)"""
    assert_accepted(
        {
            "MODULE_NAME": ["Foo"],
            "HEADERS": [STUB_HPP],
            "DEPENDS": ["Fw_Types"],
            "CMAKE_ADD_OPTIONS": ["INTERFACE"],
        },
        args=["Foo", "INTERFACE", "HEADERS", STUB_HPP, "DEPENDS", "Fw_Types"],
        additional=LIBRARY_CONTROLS,
    )


# ---- List directives ----


@pytest.mark.parametrize(
    "directive,values",
    [
        ("HEADERS", [STUB_HPP, STUB_CPP]),
        ("SOURCES", [STUB_CPP, STUB2_CPP]),
        ("DEPENDS", ["Fw_Types", "Fw_Com"]),
        ("AUTOCODER_INPUTS", [MODEL_FPP, STUB_CPP]),
        ("REQUIRES_IMPLEMENTATIONS", ["Os_File", "Os_Task"]),
        ("LINK_DEPENDS", [STUB_LD, STUB_CPP]),
    ],
)
def test_list_directive(directive, values):
    """Each list directive captures its values in order"""
    assert_accepted(
        base_outputs(**{directive: values}), args=["Foo", directive] + values
    )


def test_directive_transitions():
    """Values attach to the most recent directive across a full chain"""
    assert_accepted(
        base_outputs(
            SOURCES=[STUB_CPP],
            HEADERS=[STUB_HPP],
            DEPENDS=["Fw_Types", "Fw_Com"],
            AUTOCODER_INPUTS=[MODEL_FPP],
            LINK_DEPENDS=[STUB_LD],
        ),
        args=[
            "Foo",
            "SOURCES",
            STUB_CPP,
            "HEADERS",
            STUB_HPP,
            "DEPENDS",
            "Fw_Types",
            "Fw_Com",
            "AUTOCODER_INPUTS",
            MODEL_FPP,
            "LINK_DEPENDS",
            STUB_LD,
        ],
    )


@pytest.mark.parametrize("directive", LIST_DIRECTIVES)
def test_trailing_empty_directive_rejected(directive):
    """A list directive given no values before the arguments end is an error"""
    assert_rejected(
        f"{directive} supplied without values",
        args=["Foo"] + other_directive(directive) + [directive],
    )


@pytest.mark.parametrize("directive", LIST_DIRECTIVES)
def test_middle_empty_directive_rejected(directive):
    """A list directive given no values before the next directive is an error"""
    assert_rejected(
        f"{directive} supplied without values",
        args=["Foo", directive] + other_directive(directive),
    )


def test_bare_sources_rejected():
    """SOURCES with no values is an error rather than an omitted SOURCES"""
    assert_rejected("SOURCES supplied without values", args=["Foo", "SOURCES"])


def test_empty_additional_directive_rejected():
    """List directives declared by the calling API are subject to the same check"""
    assert_rejected(
        "CHOOSES_IMPLEMENTATIONS supplied without values",
        args=["Foo", "SOURCES", STUB_CPP, "CHOOSES_IMPLEMENTATIONS"],
        module_type="Unit Test",
        additional=UT_CONTROLS,
    )


def test_empty_flag_before_end_is_true():
    """Zero-argument flags are exempt from the empty-directive check"""
    assert_accepted(
        base_outputs(SOURCES=[STUB_CPP], UT_AUTO_HELPERS=["TRUE"]),
        args=["Foo", "SOURCES", STUB_CPP, "UT_AUTO_HELPERS"],
        module_type="Unit Test",
        additional=UT_CONTROLS,
    )


def test_undeclared_directive_is_a_value():
    """Control words not declared by the calling API are ordinary values"""
    assert_rejected(
        "INTERFACE does not exist but was specified as a SOURCES",
        args=["Foo", "SOURCES", STUB_CPP, "INTERFACE"],
    )


# ---- Path handling ----


def test_relative_path_resolved():
    """Relative paths resolve against the calling directory to absolute real paths"""
    assert_accepted(
        base_outputs(SOURCES=[STUB_CPP], HEADERS=[STUB_HPP]),
        args=[
            "Foo",
            "SOURCES",
            "stub.cpp",
            "HEADERS",
            "../TestDirectiveParser/stub.hpp",
        ],
    )


@pytest.mark.parametrize(
    "directive", ["HEADERS", "SOURCES", "AUTOCODER_INPUTS", "LINK_DEPENDS"]
)
def test_missing_file(directive):
    """File directives reject paths that do not exist"""
    assert_rejected(
        f"does not exist but was specified as a {directive}",
        args=["Foo", directive, STUB_CPP, MISSING],
    )


def test_depends_not_checked_as_file():
    """DEPENDS names targets, so nonexistent paths are not rejected"""
    assert_accepted(
        base_outputs(SOURCES=[STUB_CPP], DEPENDS=[MISSING]),
        args=["Foo", "SOURCES", STUB_CPP, "DEPENDS", MISSING],
    )


# ---- Flag directives ----


@pytest.mark.parametrize(
    "args",
    [
        ["Foo", "EXCLUDE_FROM_ALL", "SOURCES", STUB_CPP, "DEPENDS", "Fw_Types"],
        ["Foo", "SOURCES", STUB_CPP, "EXCLUDE_FROM_ALL", "DEPENDS", "Fw_Types"],
        ["Foo", "SOURCES", STUB_CPP, "DEPENDS", "Fw_Types", "EXCLUDE_FROM_ALL"],
    ],
    ids=["first", "middle", "last"],
)
def test_flag_position(args):
    """A zero-argument flag is rolled into CMAKE_ADD_OPTIONS wherever it appears"""
    assert_accepted(
        {
            "MODULE_NAME": ["Foo"],
            "SOURCES": [STUB_CPP],
            "DEPENDS": ["Fw_Types"],
            "CMAKE_ADD_OPTIONS": ["EXCLUDE_FROM_ALL"],
        },
        args=args,
    )


def test_consecutive_flags():
    """Adjacent flags are each recorded; roll-up order follows the control-set order"""
    assert_accepted(
        {
            "MODULE_NAME": ["Foo"],
            "SOURCES": [STUB_CPP],
            "CMAKE_ADD_OPTIONS": ["EXCLUDE_FROM_ALL", "INTERFACE", "OBJECT"],
        },
        args=["Foo", "OBJECT", "INTERFACE", "EXCLUDE_FROM_ALL", "SOURCES", STUB_CPP],
        additional=LIBRARY_CONTROLS,
    )


def test_flag_with_explicit_boolean():
    """A CMake add-option flag may carry a boolean; only truthy values are rolled up"""
    assert_accepted(
        {
            "MODULE_NAME": ["Foo"],
            "SOURCES": [STUB_CPP],
            "CMAKE_ADD_OPTIONS": ["EXCLUDE_FROM_ALL"],
        },
        args=["Foo", "EXCLUDE_FROM_ALL", "ON", "SOURCES", STUB_CPP],
    )
    assert_accepted(
        {
            "MODULE_NAME": ["Foo"],
            "SOURCES": [STUB_CPP],
            "EXCLUDE_FROM_ALL": ["OFF"],
            "CMAKE_ADD_OPTIONS": [],
        },
        args=["Foo", "EXCLUDE_FROM_ALL", "OFF", "SOURCES", STUB_CPP],
    )


def test_non_cmake_flags_become_true():
    """Flags that are not add_library/add_executable options export TRUE, not an add-option"""
    assert_accepted(
        {
            "MODULE_NAME": ["Foo"],
            "SOURCES": [STUB_CPP],
            "UT_AUTO_HELPERS": ["TRUE"],
            "INCLUDE_GTEST": ["TRUE"],
            "CHOOSES_IMPLEMENTATIONS": ["Os_File_Posix"],
            "CMAKE_ADD_OPTIONS": [],
        },
        args=[
            "Foo",
            "UT_AUTO_HELPERS",
            "SOURCES",
            STUB_CPP,
            "CHOOSES_IMPLEMENTATIONS",
            "Os_File_Posix",
            "INCLUDE_GTEST",
        ],
        module_type="Unit Test",
        additional=UT_CONTROLS,
    )


@pytest.mark.parametrize("flag", ["GLOBAL_IMPLICIT_DEPENDENCY", "BASE_CONFIG"])
def test_config_global_flags(flag):
    """The register_fprime_config global-interface flags are zero-argument flags exporting TRUE"""
    assert_accepted(
        {
            "MODULE_NAME": ["Foo"],
            "HEADERS": [STUB_HPP],
            flag: ["TRUE"],
            "CMAKE_ADD_OPTIONS": [],
        },
        args=["Foo", "HEADERS", STUB_HPP, flag],
        additional=CONFIG_CONTROLS,
    )


# ---- Duplicate directives (regression for nasa/fprime#5909) ----


@pytest.mark.parametrize(
    "directive,value",
    [
        ("HEADERS", STUB_HPP),
        ("SOURCES", STUB2_CPP),
        ("DEPENDS", "Fw_Types"),
        ("AUTOCODER_INPUTS", MODEL_FPP),
        ("REQUIRES_IMPLEMENTATIONS", "Os_File"),
        ("LINK_DEPENDS", STUB_LD),
    ],
)
def test_duplicate_list_directive(directive, value):
    """Each list directive is rejected when supplied twice"""
    assert_rejected(
        f"{directive} supplied multiple times",
        args=["Foo", "SOURCES", STUB_CPP, directive, value, directive, value],
    )


def test_duplicate_sources_separated():
    """Duplicates are detected even when other directives sit between them"""
    assert_rejected(
        "SOURCES supplied multiple times",
        args=["Foo", "SOURCES", STUB_CPP, "HEADERS", STUB_HPP, "SOURCES", STUB2_CPP],
    )


def test_duplicate_first_occurrence_empty():
    """An empty first occurrence is reported as empty before the duplicate is reached"""
    assert_rejected(
        "SOURCES supplied without values",
        args=["Foo", "SOURCES", "DEPENDS", "Fw_Types", "SOURCES", STUB_CPP],
    )


def test_duplicate_adjacent():
    """Back-to-back occurrences are rejected"""
    assert_rejected(
        "DEPENDS supplied multiple times",
        args=["Foo", "SOURCES", STUB_CPP, "DEPENDS", "DEPENDS", "Fw_Types"],
    )


def test_duplicate_flag():
    """Zero-argument flags are subject to the same check"""
    assert_rejected(
        "EXCLUDE_FROM_ALL supplied multiple times",
        args=["Foo", "EXCLUDE_FROM_ALL", "SOURCES", STUB_CPP, "EXCLUDE_FROM_ALL"],
    )


def test_duplicate_additional_flag():
    """Flags declared by the calling API (INTERFACE) are subject to the same check"""
    assert_rejected(
        "INTERFACE supplied multiple times",
        args=["Foo", "INTERFACE", "INTERFACE", "HEADERS", STUB_HPP],
        additional=LIBRARY_CONTROLS,
    )


def test_duplicate_unit_test_directive():
    """Unit-test specific directives are subject to the same check"""
    assert_rejected(
        "UT_AUTO_HELPERS supplied multiple times",
        args=["Foo", "UT_AUTO_HELPERS", "SOURCES", STUB_CPP, "UT_AUTO_HELPERS"],
        module_type="Unit Test",
        additional=UT_CONTROLS,
    )


# ---- Legacy variable API ----


def test_legacy_variables_module():
    """SOURCE_FILES/MOD_DEPS/HEADER_FILES sort buildable sources from autocoder inputs"""
    assert_accepted(
        base_outputs(
            SOURCES=[STUB_CPP],
            AUTOCODER_INPUTS=[MODEL_FPP],
            HEADERS=[STUB_HPP],
            DEPENDS=["Fw_Types"],
        ),
        args=["Foo"],
        legacy={
            "SOURCE_FILES": [STUB_CPP, MODEL_FPP],
            "MOD_DEPS": ["Fw_Types"],
            "HEADER_FILES": [STUB_HPP],
        },
    )


def test_legacy_variables_unit_test():
    """UT_SOURCE_FILES/UT_MOD_DEPS/UT_AUTO_HELPERS feed unit-test registration"""
    assert_accepted(
        {
            "MODULE_NAME": ["Bar_ut_exe"],
            "SOURCES": [STUB_CPP],
            "HEADERS": [],  # Legacy path always defines HEADERS, even when UT_HEADER_FILES is unset
            "AUTOCODER_INPUTS": [MODEL_FPP],
            "DEPENDS": ["Fw_Types"],
            "UT_AUTO_HELPERS": ["TRUE"],
            "CMAKE_ADD_OPTIONS": [],
        },
        args=[],
        module_type="Unit Test",
        additional=UT_CONTROLS,
        current_module="Bar",
        legacy={
            "UT_SOURCE_FILES": [STUB_CPP, MODEL_FPP],
            "UT_MOD_DEPS": ["Fw_Types"],
            "UT_AUTO_HELPERS": ["ON"],
        },
    )


@pytest.mark.parametrize(
    "variable,module_type",
    [
        ("SOURCE_FILES", "Library"),
        ("MOD_DEPS", "Library"),
        ("HEADER_FILES", "Library"),
        ("UT_SOURCE_FILES", "Unit Test"),
        ("UT_MOD_DEPS", "Unit Test"),
        ("UT_AUTO_HELPERS", "Unit Test"),
    ],
)
def test_legacy_variable_mixed_with_directives(variable, module_type):
    """Setting a legacy variable and passing directives in the same call is rejected"""
    assert_rejected(
        f"Cannot both set {variable}",
        args=["Foo", "SOURCES", STUB_CPP],
        module_type=module_type,
        additional=UT_CONTROLS if module_type == "Unit Test" else None,
        legacy={variable: [STUB_CPP]},
    )
