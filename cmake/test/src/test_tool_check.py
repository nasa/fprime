####
# test_basic.py:
#
# Basic CMake tests.
#
####
import os
import sys
import tempfile
import platform
import shutil
import distutils.spawn
import pytest
import cmake

def test_tools_validation():
    """
    Test the validation step for the python tools. This is done by setting PYTHON_HOME to "" and
    then checking to ensure that the build fails, and the validation message is printed.
    """
    old_home = os.environ.get("PYTHONHOME", "")
    old_path = os.environ.get("PATH", "")
    old_ppath = os.environ.get("PYTHONPATH", "")
    owd = os.getcwd()
    tmpd = tempfile.mkdtemp()
    try:
        os.chdir(tmpd)
        python_dir = distutils.spawn.find_executable("python").replace("/python","")
        with pytest.raises(AssertionError):
            os.environ["PYTHONHOME"] = ""
            os.environ["PYTHONPATH"] = ""
            os.environ["PATH"] = os.environ.get("PATH", "").replace(python_dir + ":", "").replace(":" + python_dir, "")
            ret, stdout, stderr = cmake.run_cmake(os.path.join(os.path.dirname(__file__), "..", "..", "..", "Ref"), capout=True)
            print(stdout.decode("utf-8"), file=sys.stdout)
            print(stderr.decode("utf-8"), file=sys.stderr)
            all_out = "".join([stdout.decode("utf-8"), stderr.decode("utf-8")])
            assert ret, "Assert that the CMake run has failed"
        ex_texts = ["  [VALIDATION] Validation failed for:\n  python;",
                    "cmake/support/validation/pipsetup.py"]
        for ex_text in ex_texts:
            assert ex_text in all_out, "Failed to find tool check error"
    finally:
        os.chdir(owd)
        shutil.rmtree(tmpd)
        os.environ["PATH"] = old_path
        os.environ["PYTHONHOME"] = old_home
        os.environ["PYTHONPATH"] = old_ppath
