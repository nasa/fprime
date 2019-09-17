####
# run_deployment.py:
#
# Runs a deployment. Starts a GUI, a TCPServer, and the deployment application.
####
from __future__ import print_function
import os
import sys
import copy
import time
import atexit
import argparse
import platform
import signal
import subprocess
import shutil
import distutils.spawn
import webbrowser
import fprime_gds.executables.cli

# Try to import each GUI type, and if it can be imported
# it will be provided to the user as an option
GUIS = ["none", "html"]
try:
    import fprime_gds.tkgui.tools.gse
    GUIS.append("tk")
except ImportError as exc:
    print("[WARNING] Could not import TK GUI: {0}:{1}".format(type(exc), str(exc), file=sys.stderr))
except SyntaxError as exc:
    pass
try:
    import fprime_gds.wxgui.tools.gds
    GUIS.append("wx")
except ImportError as exc:
    pass

def detect_terminal(title):
    '''
    Detect a terminal based on the OS.
    @param title: title to put on the terminal
    :return: terminal program
    '''
    if platform.system() == "Windows":
        return ["cmd.exe", "/C"]
    elif platform.system() == "Darwin":
        return [os.path.join(os.path.dirname(__file__), "..", "..", "..",
                             "bin", "osx", "terminal-wrapper.bash")]
    else:
        terminals = [["gnome-terminal", "--disable-factory", "--title", title, "--"],
            ["xterm", "-hold", "-T", title, "-e"], ["lxterminal", "-t", title, "-e"], ["Konsole", "-e"],
            ["x-terminal-emulator", "-T", title, "-e"]]
        # Gnome terminal not working
        for terminal in terminals:
            try:
                if shutil.which(terminal[0]) is not None:
                    return terminal
            except AttributeError:
                if distutils.spawn.find_executable(terminal[0]) is not None:
                    return terminal
        else:
            error_exit("No terminal emulator found. Please install 'xterm'", 8)


def get_args(args):
    '''
    Gets an argument parsers to read the command line and process the arguments. Return
    the arguments in their namespace.
    '''
    parser = argparse.ArgumentParser(description='Run F´ deployment with: GDS data server, GDS GUI, and application.')
    parser.add_argument("-g", "--gui", choices=GUIS, dest="gui", type=str,
                        help="Set the desired GUI system for running the deployment. [default: %(default)s]",
                        default="tk" if "tk" in GUIS else "wx")
    parser.add_argument("-n", "--no-app", dest="noapp", action="store_true", default=False,
                        help="Do not run deployment binary. Overrides --app.")
    # Get custom handlers for all executables we are running
    gds_handler = fprime_gds.executables.cli.GdsParser()
    tts_handler = fprime_gds.executables.cli.MiddleWareParser()
    app_handler = fprime_gds.executables.cli.BinaryDeployment()
    # Add all needed arguments
    gds_handler.add_args(parser)
    tts_handler.add_args(parser)
    app_handler.add_args(parser)

    # Parse the arguments, and refine through all handlers
    try:
        parsed_args = parser.parse_args(args)
        values = tts_handler.refine_args(parsed_args)
        values.update(gds_handler.refine_args(parsed_args))
        # Don't refine app, if no-app specified
        if not parsed_args.noapp:
            values.update(app_handler.refine_args(parsed_args))
        # Special checks
        if values["config"].get_file_path() is None and parsed_args.gui == "wx":
            raise ValueError("Must supply --config when using 'wx' GUI.")
    # On ValueError print error, help and exit
    except ValueError as vexc:
        print("[ERROR] {}".format(str(vexc)), file=sys.stderr, end="\n\n")
        parser.print_help(sys.stderr)
        sys.exit(-1)
    values["gui"] = parsed_args.gui
    return values


def error_exit(error, code=1):
    '''
    Error this run, and exit cleanly.
    '''
    print("[ERROR] {0}".format(error), file=sys.stderr)
    sys.exit(code)


def launch_process(cmd, stdout=None, stderr=None, name=None, pgroup=False, env=None):
    '''
    Launch a process in python
    :param cmd: command arguments to run
    :param stdout: standard out destination
    :param stderr: standard error destination
    :param name: (optional) short name for printing
    :param env: (optional) environment to run in
    :return: running process
    '''
    if name is None:
        name = str(cmd)
    # Start with PGroup to nuke poorly designed terminals
    if pgroup:
        proc = subprocess.Popen(cmd, stdout=stdout, stderr=stderr, shell=False, preexec_fn=os.setpgrp, env=env)
    else:
        proc = subprocess.Popen(cmd, stdout=stdout, stderr=stderr, shell=False, env=env)

    # When python exits, nuke this process
    def kill_wait():
        '''Kill process and wait it to die'''
        try:
            # Process groups, and their terminal spawn deserve no mercy
            if pgroup:
                os.killpg(proc.pid, signal.SIGINT)
                os.killpg(proc.pid, signal.SIGTERM)
                time.sleep(2)
                os.killpg(proc.pid, signal.SIGKILL)
                return
            # Normal processes are gently terminated
            proc.terminate()
            time.sleep(1)
            # Terminate with extreme prejudice
            proc.kill()
        except OSError as ose:
            pass
    atexit.register(kill_wait)
    print("[INFO] Waiting 5 seconds for {0} to start".format(name))
    time.sleep(5)
    proc.poll()
    if proc.returncode is not None:
        error_exit("Failed to start {0}".format(name), 1)
        raise Exception("FAILED TO EXIT")
    return proc


def launch_tts(port, address, logs):
    '''
    Launch the Threaded TCP Server
    :param port: port to attach to
    :param address: address to bind to
    :param logs: logs output directory
    :return: process
    '''
    # Open log, and prepare to close it cleanly on exit
    tts_log = open(os.path.join(logs, "ThreadedTCP.log"), 'w')
    atexit.register(lambda: tts_log.close())
    # Launch the tcp server
    tts_cmd = ["python", "-u", "-m", "fprime_gds.executables.tcpserver", "--port", str(port), "--host", str(address)]
    return launch_process(tts_cmd, stdout=tts_log, stderr=subprocess.STDOUT, name="TCP Server")


def launch_tk(port, dictionary, address, log_dir):
    '''
    Launch the GSE
    :param port: port to connect to
    :param dictionary: dictionary to use
    :param address: dictionary to connect to
    :param log_dir: directory to log to
    :return: process
    '''
    gse_args = ["python", "-u", "-m", "fprime_gds.tkgui.tools.gse", "--port", str(port)]
    if os.path.isfile(dictionary):
        gse_args.extend(["-x", dictionary])
    elif os.path.isdir(dictionary):
        gse_args.extend(["--dictionary", dictionary])
    else:
        print("[ERROR] Dictionary invalid, must be XML or PY dicts: {0}".format(dictionary), file=sys.stderr)
    gse_args.extend(["--connect", "--addr", address, "-L", log_dir])
    return launch_process(gse_args, name="TK GUI")


def launch_wx(port, dictionary, address, log_dir, config):
    '''
    Launch the GDS gui
    :param port: port to connect to
    :param dictionary: dictionary to look at
    :param address: address to connect to
    :param log_dir: directory to place logs
    :param config: configuration to use
    :return: process
    '''
    gse_args = ["python", "-u", "-m", "fprime_gds.wxgui.tools.gds", "--port", str(port)]
    if os.path.isfile(dictionary):
        gse_args.extend(["-x", dictionary])
    elif os.path.isdir(dictionary):
        gse_args.extend(["--dictionary", dictionary])
    else:
        print("[ERROR] Dictionary invalid, must be XML or PY dicts: {0}".format(dictionary), file=sys.stderr)
    # For OSX, add in the wx wrapper
    if platform.system() == "Darwin":
        gse_args.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "bin", "osx", "wx-wrapper.bash"))
    gse_args.extend(["--addr", address, "-L", log_dir, "--config", config.get_file_path()])
    return launch_process(gse_args, name="WX GUI")

def launch_html(port, dictionary, address, log_dir, config):
    '''
    Launch the flask server and a browser pointed at the HTML page.
    :param port: port to connect to
    :param dictionary: dictionary to look at
    :param address: address to connect to
    :param log_dir: directory to place logs
    :param config: configuration to use
    :return: process
    '''
    gse_env = copy.copy(os.environ).update(
        {"DICTIONARY": str(dictionary), "FLASK_APP": "fprime_gds.flask.app", "FSW_PORT": str(port)})
    gse_args = ["python", "-u", "-m", "flask", "run"]
    ret = launch_process(gse_args, name="HTML GUI", env=gse_env)
    time.sleep(2)
    webbrowser.open("http://localhost:5000/", new=0, autoraise=True)
    return ret

def launch_app(app, port, address, log_dir):
    '''
    Launch the app
    :param app: application to launch
    :param port: port to connect to
    :param address: address to connect to
    :param log_dir: log directory to place files into
    :return: process
    '''
    app_name = os.path.basename(app)
    app_cmd = detect_terminal(app_name)
    app_cmd.extend(["python", "-u", "-m", "fprime_gds.executables.appwrapper",
                    os.path.join(log_dir, "{0}.stdout.log".format(app_name)),
                    ])
    app_cmd.extend([os.path.abspath(app), "-p", str(port), "-a", address])
    return launch_process(app_cmd, name="{0} Application".format(app_name), pgroup=True)


def main(argv=None):
    '''
    Main function used to launch processes.
    '''
    wait_proc = None
    args = get_args(argv)
    # Launch TTS and ensure it starts w/o error
    launch_tts(args["port"], args["address"], args["logs"])
    # Launch a gui, if specified
    address = args["address"]
    if address == "0.0.0.0":
        address = "127.0.0.1"
    # Launch the desired GUI package
    gui = args.get("gui", "none")
    if gui == "tk":
        wait_proc = launch_tk(args["port"], args["dictionary"], address, args["logs"])
    elif gui == "wx":
        wait_proc = launch_wx(args["port"], args["dictionary"], address, args["logs"], args["config"])
    elif gui == "html":
        wait_proc = launch_html(args["port"], args["dictionary"], address, args["logs"], args["config"])
    elif gui == "none":
        print("[WARNING] No GUI specified, running headless", file=sys.stderr)
    else:
        raise Exception("Invalid GUI specified: {0}".format(args["gui"]))
    # Run app if specified
    if "app" in args:
        app_proc = launch_app(args["app"], args["port"], address, args["logs"])
    # Determine waitable item
    if wait_proc is None:
        wait_proc = app_proc
    # Wait for either gui or app to finish, then close
    try:
        print("[INFO] F´ is now running. CTRL-C to shutdown all components.")
        wait_proc.wait()
    except KeyboardInterrupt:
        print("[INFO] CTRL-C received. Exiting.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
