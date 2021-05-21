####
# run_deployment.py:
#
# Runs a deployment. Starts a GUI, a TCPServer, and the deployment application.
####
import os
import sys
import platform
import webbrowser
from pathlib import Path
from fprime.fbuild.settings import (
    IniSettings,
    FprimeLocationUnknownException,
    FprimeSettingsException,
)

import fprime_gds.executables.cli
import fprime_gds.executables.utils


def get_artifacts_root() -> Path:
    try:
        ini_settings = IniSettings.load(None, Path.cwd())
    except FprimeLocationUnknownException:
        print(
            "[ERROR] Not in fprime deployment and no artifacts root provided, unable to find dictionary and/or app"
        )
        sys.exit(-1)
    except FprimeSettingsException as e:
        print("[ERROR]", e)
        sys.exit(-1)
    assert "install_dest" in ini_settings, "install_dest not in settings.ini"
    print(
        "[INFO] Autodetected artifacts root '{}' from deployment settings.ini file.".format(
            ini_settings["install_dest"]
        )
    )
    return ini_settings["install_dest"]


def find_app(root: Path) -> Path:
    bin_dir = root / "bin"

    if not bin_dir.exists():
        print("[ERROR] binary location {} does not exist".format(bin_dir))
        sys.exit(-1)

    files = []
    for child in bin_dir.iterdir():
        if child.is_file():
            files.append(child)

    if len(files) == 0:
        print("[ERROR] No app found in binary location {}".format(bin_dir))
        sys.exit(-1)

    if len(files) > 1:
        print(
            "[ERROR] Multiple app candidates in binary location {}. Specify app manually with --app.".format(
                bin_dir
            )
        )
        sys.exit(-1)

    return files[0]


def find_dict(root: Path) -> Path:
    dict_dir = root / "dict"

    if not dict_dir.exists():
        print("[ERROR] dictionary location {} does not exist".format(dict_dir))
        sys.exit(-1)

    files = []
    for child in dict_dir.iterdir():
        if child.is_file() and child.suffix == ".xml":
            files.append(child)

    if len(files) == 0:
        print(
            "[ERROR] No xml dictionary found in dictionary location {}".format(dict_dir)
        )
        sys.exit(-1)

    if len(files) > 1:
        print(
            "[ERROR] Multiple xml dictionaries found in dictionary location {}. Specify dictionary manually with --dictionary.".format(
                dict_dir
            )
        )
        sys.exit(-1)

    return files[0]


def get_settings():
    args = parse_args()

    if args.dictionary is not None and (args.app is not None or args.noapp):
        return args

    root = None
    if args.root_dir is not None:
        root = Path(args.root_dir)
    else:
        root = get_artifacts_root()
    root = root / platform.system()

    if not args.noapp and args.app is None:
        args.app = find_app(root)

    if args.dictionary is None:
        args.dictionary = find_dict(root)

    return args


def parse_args():
    """
    Gets an argument parsers to read the command line and process the arguments. Return
    the arguments in their namespace.

    :return: parsed argument namespace
    """
    # Get custom handlers for all executables we are running
    arg_handlers = [
        fprime_gds.executables.cli.LogDeployParser,
        fprime_gds.executables.cli.GdsParser,
        fprime_gds.executables.cli.MiddleWareParser,
        fprime_gds.executables.cli.BinaryDeployment,
        fprime_gds.executables.cli.CommParser,
    ]
    # Parse the arguments, and refine through all handlers
    try:
        args, parser = fprime_gds.executables.cli.ParserBase.parse_args(
            arg_handlers, "Run F prime deployment and GDS"
        )
        # Special checks
        if args.config.get_file_path() is None and args.gui == "wx":
            raise ValueError("Must supply --config when using 'wx' GUI.")
    # On ValueError print error, help and exit
    except ValueError as vexc:
        print("[ERROR] {}".format(str(vexc)), file=sys.stderr, end="\n\n")
        parser.print_help(sys.stderr)
        sys.exit(-1)
    return args


def launch_process(cmd, logfile=None, name=None, env=None, launch_time=5):
    """
    Launch a child subprocess. This subprocess will allow the child to run outside of the memory context of Python.

    :param cmd: list of command arguments to run by handing to subprocess.
    :param logfile: (optional) place to redirect output to for purposes of logging. Default: None, screen.
    :param name: (optional) short name for printing messages.
    :param env: (optional) environment to run in. Allows for special environment contexts.
    :param launch_time: (optional) time to launch the process, before rendering an error.
    :return: running process
    """
    if name is None:
        name = str(cmd)
    print(
        "[INFO] Ensuring {} is stable for at least {} seconds".format(name, launch_time)
    )
    try:
        return fprime_gds.executables.utils.run_wrapped_application(
            cmd, logfile, env, launch_time
        )
    except fprime_gds.executables.utils.AppWrapperException as awe:
        print("[ERROR] {}.".format(str(awe)), file=sys.stderr)
        try:
            if logfile is not None:
                with open(logfile) as file_handle:
                    for line in file_handle.readlines():
                        print("    [LOG] {}".format(line.strip()), file=sys.stderr)
        except Exception:
            pass
        raise fprime_gds.executables.utils.AppWrapperException(
            "Failed to run {}".format(name)
        )


def launch_tts(tts_port, tts_addr, logs, **_):
    """
    Launch the Threaded TCP Server

    :param tts_port: port to attach to
    :param tts_addr: address to bind to
    :param logs: logs output directory
    :return: process
    """
    # Open log, and prepare to close it cleanly on exit
    tts_log = os.path.join(logs, "ThreadedTCP.log")
    # Launch the tcp server
    tts_cmd = [
        sys.executable,
        "-u",
        "-m",
        "fprime_gds.executables.tcpserver",
        "--port",
        str(tts_port),
        "--host",
        str(tts_addr),
    ]
    return launch_process(tts_cmd, logfile=tts_log, name="TCP Server")


def launch_wx(port, dictionary, connect_address, log_dir, config, **_):
    """
    Launch the GDS gui

    :param port: port to connect to
    :param dictionary: dictionary to look at
    :param connect_address: address to connect to
    :param log_dir: directory to place logs
    :param config: configuration to use
    :return: process
    """
    gse_args = [
        sys.executable,
        "-u",
        "-m",
        "fprime_gds.wxgui.tools.gds",
        "--port",
        str(port),
    ]
    if os.path.isfile(dictionary):
        gse_args.extend(["-x", dictionary])
    elif os.path.isdir(dictionary):
        gse_args.extend(["--dictionary", dictionary])
    else:
        print(
            "[ERROR] Dictionary invalid, must be XML or PY dicts: {}".format(
                dictionary
            ),
            file=sys.stderr,
        )
    # For macOS, add in the wx wrapper
    if platform.system() == "Darwin":
        gse_args.insert(
            0,
            os.path.join(
                os.path.dirname(__file__),
                "..",
                "..",
                "..",
                "bin",
                "osx",
                "wx-wrapper.bash",
            ),
        )
    gse_args.extend(
        ["--addr", connect_address, "-L", log_dir, "--config", config.get_file_path()]
    )
    return launch_process(gse_args, name="WX GUI")


def launch_html(tts_port, dictionary, connect_address, logs, gui_addr, gui_port, **extras):
    """
    Launch the flask server and a browser pointed at the HTML page.

    :param tts_port: port to connect to
    :param dictionary: dictionary to look at
    :param connect_address: address to connect to
    :param logs: directory to place logs
    :param gui_addr: Flask server host IP address
    :param gui_port: Flask server port number
    :return: process
    """
    gse_env = os.environ.copy()
    gse_env.update(
        {
            "DICTIONARY": str(dictionary),
            "FLASK_APP": "fprime_gds.flask.app",
            "TTS_PORT": str(tts_port),
            "TTS_ADDR": connect_address,
            "LOG_DIR": logs,
            "SERVE_LOGS": "YES",
        }
    )
    gse_args = [sys.executable, "-u", "-m", "flask", "run", 
                "--host", str(gui_addr), 
                "--port", str(gui_port)]
    ret = launch_process(gse_args, name="HTML GUI", env=gse_env, launch_time=2)
    if extras["gui"] == "html":
        webbrowser.open(f"http://{str(gui_addr)}:{str(gui_port)}/", new=0, autoraise=True)
    return ret


def launch_app(app, port, address, logs, **_):
    """
    Launch the app

    :param app: application to launch
    :param port: port to connect to
    :param address: address to connect to
    :param logs: log directory to place files into
    :return: process
    """
    app_name = os.path.basename(app)
    logfile = os.path.join(logs, "{}.log".format(app_name))
    app_cmd = [os.path.abspath(app), "-p", str(port), "-a", address]
    return launch_process(
        app_cmd, name="{} Application".format(app_name), logfile=logfile, launch_time=1
    )


def launch_comm(comm_adapter, tts_port, connect_address, logs, **all_args):
    """

    :return:
    """

    app_cmd = [
        sys.executable,
        "-u",
        "-m",
        "fprime_gds.executables.comm",
        "--tts-addr",
        connect_address,
        "--tts-port",
        str(tts_port),
        "-l",
        logs,
        "--log-directly",
        "--comm-adapter",
        all_args["adapter"],
    ]
    # Manufacture arguments for the selected adapter
    for arg in comm_adapter.get_arguments().keys():
        definition = comm_adapter.get_arguments()[arg]
        destination = definition["dest"]
        app_cmd.append(arg[0])
        app_cmd.append(str(all_args[destination]))
    return launch_process(
        app_cmd,
        name="{} Application".format("comm[{}]".format(all_args["adapter"])),
        launch_time=1,
    )


def main():
    """
    Main function used to launch processes.
    """
    settings = vars(get_settings())
    # Launch a gui, if specified
    settings["connect_address"] = (
        settings["tts_addr"] if settings["tts_addr"] != "0.0.0.0" else "127.0.0.1"
    )
    # List of things to launch, in order.
    launchers = [launch_tts, launch_comm]
    # Add app, if possible
    if settings.get("app", None) is not None and settings.get("adapter", "") == "ip":
        launchers.append(launch_app)
    elif settings.get("app", None) is not None:
        print("[WARNING] App cannot be auto-launched without IP adapter")

    # Launch the desired GUI package
    gui = settings.get("gui", "none")
    if gui == "wx":
        launchers.append(launch_wx)
    elif gui == "html" or gui == "none":
        launchers.append(launch_html)
    # elif gui == "none":
    #    print("[WARNING] No GUI specified, running headless", file=sys.stderr)
    else:
        raise Exception("Invalid GUI specified: {}".format(settings["gui"]))
    # Launch launchers and wait for the last app to finish
    try:
        procs = []
        for launcher in launchers:
            procs.append(launcher(**settings))
        print("[INFO] F prime is now running. CTRL-C to shutdown all components.")
        procs[-1].wait()
    except KeyboardInterrupt:
        print("[INFO] CTRL-C received. Exiting.")
    except Exception as exc:
        print(
            "[INFO] Shutting down F prime due to error. {}".format(str(exc)),
            file=sys.stderr,
        )
        return 1
    # Processes are killed atexit
    return 0


if __name__ == "__main__":
    sys.exit(main())
