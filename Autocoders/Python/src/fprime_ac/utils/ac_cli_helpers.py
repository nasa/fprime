import os
import sys
import subprocess
import argparse

# All possible argument specifications:
#     <full-name> : <arg parse key/val map>
POSSIBLE_ARGUMENTS = {
    "input_files": {"help": "Input Ai file, run through the Autocoder", "nargs": "+"},
    "output_dir": {"help": "Directory to place outputs of the Autocoder run", "default": None},
    "dict_format": {"help": "Format of dictionaries to produce", "choices": ["xml", "python"]},
    "build_root": {"help": "Set BUILD_ROOT variable.", "default": os.environ.get("BUILD_ROOT", ">>NONE<<")}
}


def parse_args(args, specs=[], desc="{0}: F′ Autocoder helper".format(os.path.basename(__file__))):
    """
    Parse the input argument list suppling the needed arguments to read from the
    global (common) specification above.
    :param args: list of input arguments for parsing
    :param specs: list of argument specs to use
    :param desc: command line description
    """
    parser = argparse.ArgumentParser(desc)
    for spec in specs:
        parser.add_argument("-{0}".format(spec[0]), "--{0}".format(spec),
                            **POSSIBLE_ARGUMENTS[spec])
    parsed = parser.parse_args(args)
    # Go through arguments and check existences
    if hasattr(parsed, "build_root") and parsed.build_root is not None and not os.path.isdir(parsed.build_root):
        print("[ERROR] --build_root, {0}, is not a valid directory".format(parsed.build_root), file=sys.stderr)
        sys.exit(1)
    elif hasattr(parsed, "build_root") and parsed.build_root is not None:
        os.environ["BUILD_ROOT"] = parsed.build_root
    # Process input argument
    if hasattr(parsed, "input_files"):
        for input_file in parsed.input_files:
            if not os.path.isfile(input_file):
                print("[ERROR] --input_files argument: {0} is not a valid file".format(input_file), file=sys.stderr)
                sys.exit(1)
    if hasattr(parsed, "output_dir") and parsed.output_dir is None:
        parsed.output_dir = os.path.dirname(parsed.input_files[0])
    return parsed
        

def run_cli(*args, **kwargs):
    """
    Run the auto-coder CLI.  This helps pass in the arguments needed to do it.
    @param args: arguments to the codegen cli
    """
    codegen = [os.path.join(os.path.dirname(__file__), "..", "..", "..", "bin", "codegen.py")]
    codegen.extend(args)
    subp = subprocess.Popen(codegen, **kwargs)
    ret = subp.wait()
    if ret != 0:
        print("[ERROR] codegen.py failed with return code: {0}".format(ret))
    return ret
