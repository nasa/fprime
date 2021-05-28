""" Cookie cutter wrapper used to template out components
"""
import os
import glob
import sys
from pathlib import Path
from typing import Dict

from cookiecutter.main import cookiecutter
from cookiecutter.exceptions import OutputDirExistsException

from fprime.fbuild.builder import Build, Target
from fprime.fbuild.cmake import CMakeExecutionException


def confirm(msg):
    """ Confirms the removal of the file with a yes or no input """
    # Loop "forever" intended
    while True:
        confirm_input = input(msg)
        if confirm_input.lower() in ["y", "yes"]:
            return True
        if confirm_input.lower() in ["n", "no"]:
            return False
        print("{} is invalid.  Please use 'yes' or 'no'".format(confirm_input))


def run_impl(deployment: Path, path: Path, platform: str, verbose: bool):
    """ Run implementation of files one time """
    target = Target.get_target("impl", set())
    build = Build(target.build_type, deployment, verbose=verbose)
    build.load(path, platform)

    hpp_files = glob.glob("{}/*.hpp".format(path), recursive=False)
    cpp_files = glob.glob("{}/*.cpp".format(path), recursive=False)
    cpp_files.sort(key=len)

    # Check destinations
    if not hpp_files or not cpp_files:
        print(
            "[WARNING] Failed to find .cpp and .hpp destination files for implementation."
        )
        return False

    common = [name for name in cpp_files if "Common" in name] + []
    hpp_dest = hpp_files[0]
    cpp_dest = common[0] if common else cpp_files[0]

    if not confirm(
        "Generate implementations and merge into {} and {}?".format(hpp_dest, cpp_dest)
    ):
        return False
    build.execute(target, context=path, make_args={})

    hpp_files_template = glob.glob("{}/*.hpp-template".format(path), recursive=False)
    cpp_files_template = glob.glob("{}/*.cpp-template".format(path), recursive=False)

    if not hpp_files_template or not cpp_files_template:
        print("[WARNING] Failed to find generated .cpp-template or .hpp-template files")
        return False

    hpp_src = hpp_files_template[0]
    cpp_src = cpp_files_template[0]

    # Copy files without headers
    for src, dest in [(hpp_src, hpp_dest), (cpp_src, cpp_dest)]:
        with open(src, "r") as file_handle:
            lines = file_handle.readlines()
        lines = lines[11:]  # Remove old header
        with open(dest, "a") as file_handle:
            file_handle.write("".join(lines))
    removals = [path / "mod.mk", path / "Makefile", Path(hpp_src), Path(cpp_src)]
    for removal in removals:
        os.remove(removal)
    return True


def add_to_cmake(list_file: Path, comp_path: Path):
    """ Adds new component to CMakeLists.txt"""
    print("[INFO] Found CMakeLists.txt at '{}'".format(list_file))
    with open(list_file, "r") as file_handle:
        lines = file_handle.readlines()
    topology_lines = [
        (line, text) for line, text in enumerate(lines) if "/Top/" in text
    ]
    line = len(topology_lines)
    if topology_lines:
        line, text = topology_lines[0]
        print(
            "[INFO] Topology inclusion '{}' found on line {}.".format(
                text.strip(), line + 1
            )
        )
    if not confirm(
        "Add component {} to {} {}?".format(
            comp_path,
            list_file,
            "at end of file" if not topology_lines else " before topology inclusion",
        )
    ):
        return False

    addition = 'add_fprime_subdirectory("${{CMAKE_CURRENT_LIST_DIR}}/{}/")\n'.format(
        comp_path
    )
    lines.insert(line, addition)
    with open(list_file, "w") as file_handle:
        file_handle.write("".join(lines))
    return True


def find_nearest_cmake_lists(component_dir: Path, deployment: Path, proj_root: Path):
    """Find the nearest CMakeLists.txt file

    The "nearest" file is defined as the closes parent that is not the "project root" that contains a CMakeLists.txt. If
    none is found, the same procedure is run from the deployment directory and includes the project root this time. If
    nothing is found, None is returned.

    In short the following in order of preference:
     - Any Component Parent
     - Any Deployment Parent
     - Project Root
     - None

    Args:
        component_dir: directory of new component
        deployment: deployment directory
        proj_root: project root directory

    Returns:
        path to CMakeLists.txt or None
    """
    test_path = component_dir.parent
    # First iterate from where we are, then from the deployment to find the nearest CMakeList.txt nearby
    for test_path, end_path in [(test_path, proj_root), (deployment, proj_root.parent)]:
        while proj_root is not None and test_path != proj_root.parent:
            cmake_list_file = test_path / "CMakeLists.txt"
            if cmake_list_file.is_file():
                return cmake_list_file
            test_path = test_path.parent
    return None


def new_component(
    path: Path, deployment: Path, platform: str, verbose: bool, settings: Dict[str, str]
):
    """ Uses cookiecutter for making new components """
    try:
        print("[WARNING] **** fprime-util new is prototype functionality ****")
        calculated_defaults = {}
        proj_root = None
        try:
            proj_root = Path(settings.get("project_root", None))
            print(proj_root)
            comp_parent_path = path.relative_to(proj_root)
            back_path = os.sep.join([".." for _ in str(comp_parent_path).split(os.sep)])
            calculated_defaults["component_path"] = str(comp_parent_path).rstrip(os.sep)
            calculated_defaults["component_path_to_fprime_root"] = str(
                back_path
            ).rstrip(os.sep)
        except (ValueError, TypeError):
            print(
                "[WARNING] No found project root. Set 'component_path' and 'component_path_to_fprime_root' carefully"
            )
        source = "gh:SterlingPeet/cookiecutter-fprime-component"
        print("[INFO] Cookiecutter source: {}".format(source))
        print()
        print("----------------")
        print(
            "[INFO] Help available here: https://github.com/SterlingPeet/cookiecutter-fprime-component/blob/master/README.rst#id3"
        )
        print("----------------")
        print()
        final_component_dir = Path(
            cookiecutter(source, extra_context=calculated_defaults)
        ).resolve()
        if proj_root is None:
            print(
                "[INFO] Created component directory without adding to build system nor generating implementation {}".format(
                    final_component_dir
                )
            )
            return 0
        # Attempt to register to CMakeLists.txt
        cmake_lists_file = find_nearest_cmake_lists(
            final_component_dir, deployment, proj_root
        )
        if cmake_lists_file is None or not add_to_cmake(
            cmake_lists_file, final_component_dir.relative_to(cmake_lists_file.parent)
        ):
            print(
                "[INFO] Could not register {} with build system. Please add it and generate implementations manually.".format(
                    final_component_dir
                )
            )
            return 0
        # Attempt implementation
        if not run_impl(deployment, final_component_dir, platform, verbose):
            print(
                "[INFO] Could not generate implementations. Please do so manually.".format(
                    final_component_dir
                )
            )
            return 0
        print("[INFO] Created new component and created initial implementations.")
        print(
            "[INFO] Next run `fprime-util build{}` in {}".format(
                "" if platform is None else " " + platform, final_component_dir
            )
        )
        return 0
    except OutputDirExistsException as out_directory_error:
        print("{}".format(out_directory_error), file=sys.stderr)
    except CMakeExecutionException as exc:
        print("[ERROR] Failed to create component. {}".format(exc), file=sys.stderr)
    except OSError as ose:
        print("[ERROR] {}".format(ose))
    return 1
