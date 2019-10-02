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
import subprocess
import webbrowser
import fprime_gds.executables.cli


def get_args(args):
    '''
    Gets an argument parsers to read the command line and process the arguments. Return
    the arguments in their namespace.
    '''
    parser = argparse.ArgumentParser(description='Run F´ deployment with: GDS data server, GDS GUI, and application.')
    # Get custom handlers for all executables we are running
    arg_handlers = [fprime_gds.executables.cli.GdsParser, fprime_gds.executables.cli.MiddleWareParser,
                    fprime_gds.executables.cli.BinaryDeployment, fprime_gds.executables.cli.CommParser]
    # Add all handlers
    for handler in arg_handlers:
        handler.add_args(parser)

    # Parse the arguments, and refine through all handlers
    try:
        parsed_args = parser.parse_args(args)
        # Add all values to the values list
        values = fprime_gds.executables.cli.refine(parser, parsed_args)
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


def launch_process(cmd, stdout=None, stderr=None, name=None, env=None, launch_time=5):
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
    # Starts the process
    proc = subprocess.Popen(cmd, stdout=stdout, stderr=stderr, shell=False, env=env)
    # When python exits, nuke this process
    def kill_wait():
        '''Kill process and wait it to die'''
        try:
            # Normal processes are gently terminated
            proc.terminate()
            time.sleep(1)
            # Terminate with extreme prejudice
            proc.kill()
        except OSError as ose:
            pass
    atexit.register(kill_wait)
    print("[INFO] Waiting {1} seconds for {0} to start".format(name, launch_time))
    time.sleep(launch_time)
    proc.poll()
    if proc.returncode is not None:
        error_exit("Failed to start {0}: '{1}'".format(name, " ".join(cmd)), 1)
        raise Exception("FAILED TO EXIT")
    return proc


def launch_tts(tts_port, tts_address, logs, **_):
    '''
    Launch the Threaded TCP Server
    :param tts_port: port to attach to
    :param tts_address: address to bind to
    :param logs: logs output directory
    :return: process
    '''
    # Open log, and prepare to close it cleanly on exit
    tts_log = open(os.path.join(logs, "ThreadedTCP.log"), 'w')
    atexit.register(lambda: tts_log.close())
    # Launch the tcp server
    tts_cmd = ["python", "-u", "-m", "fprime_gds.executables.tcpserver",
               "--port", str(tts_port), "--host", str(tts_address)]
    return launch_process(tts_cmd, stdout=tts_log, stderr=subprocess.STDOUT, name="TCP Server")


def launch_wx(port, dictionary, connect_address, log_dir, config, **_):
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
    gse_args.extend(["--addr", connect_address, "-L", log_dir, "--config", config.get_file_path()])
    return launch_process(gse_args, name="WX GUI")

def launch_html(tts_port, dictionary, connect_address, logs, **_):
    '''
    Launch the flask server and a browser pointed at the HTML page.
    :param tts_port: port to connect to
    :param dictionary: dictionary to look at
    :param connect_address: address to connect to
    :param logs: directory to place logs
    :return: process
    '''
    gse_env = copy.copy(os.environ).update({
        "DICTIONARY": str(dictionary),
        "FLASK_APP": "fprime_gds.flask.app",
        "TTS_PORT": str(tts_port),
        "TTS_ADDR": connect_address,
        "LOG_DIR": logs
    })
    gse_args = ["python", "-u", "-m", "flask", "run"]
    ret = launch_process(gse_args, name="HTML GUI", env=gse_env, launch_time=2)
    webbrowser.open("http://localhost:5000/", new=0, autoraise=True)
    return ret

def launch_app(app, port, address, logs, **_):
    '''
    Launch the app
    :param app: application to launch
    :param port: port to connect to
    :param address: address to connect to
    :param log_dir: log directory to place files into
    :return: process
    '''

    app_name = os.path.basename(app)
    o_log = open(os.path.join(logs, "{0}.log".format(app_name)), "w")
    atexit.register(lambda: o_log.close())
    app_cmd = [os.path.abspath(app), "-p", str(port), "-a", address]
    return launch_process(app_cmd, stdout=o_log, stderr=subprocess.STDOUT, name="{0} Application".format(app_name), launch_time=1)


def launch_comm(comm_adapter, tts_port, connect_address, logs, **all_args):
    """
    :return:
    """
    adapter = fprime_gds.common.adapters.base.BaseAdapter.get_adapters()[comm_adapter]
    app_cmd = ["python", "-u", "-m", "fprime_gds.executables.comm", "--tts-addr", connect_address,\
               "--tts-port", str(tts_port), "-l", logs, "--log-directly", comm_adapter]
    # Manufacture arguments for the selected adapter
    for arg in adapter.get_arguments().keys():
        definition = adapter.get_arguments()[arg]
        destination = definition["dest"]
        app_cmd.append(arg[0])
        app_cmd.append(str(all_args[destination]))
    return launch_process(app_cmd, name="{0} Application".format("comm[{}]".format(comm_adapter)), launch_time=1)

def main(argv=None):
    '''
    Main function used to launch processes.
    '''
    args = get_args(argv)
    # Launch a gui, if specified
    args["connect_address"] = args["tts_address"] if args["tts_address"] != "0.0.0.0" else "127.0.0.1"
    # List of things to launch, in order.
    launchers = [
        launch_tts,
        launch_comm
    ]
    # Add app, if possible
    if "app" in args and args.get("comm_adapter", "") == "ip":
        launchers.append(launch_app)
    elif "app" in args:
        print("[WARNING] App cannot be auto-launched without IP adapter")

    # Launch the desired GUI package
    gui = args.get("gui", "none")
    if gui == "wx":
        launchers.append(launch_wx)
    elif gui == "html":
        launchers.append(launch_html)
    elif gui == "none":
        print("[WARNING] No GUI specified, running headless", file=sys.stderr)
    else:
        raise Exception("Invalid GUI specified: {0}".format(args["gui"]))

    # Launch all launchers
    procs = []
    for launcher in launchers:
        procs.append(launcher(**args))

    # Wait for either gui or app to finish, then close
    try:
        print("[INFO] F´ is now running. CTRL-C to shutdown all components.")
        procs[-1].wait()
    except KeyboardInterrupt:
        print("[INFO] CTRL-C received. Exiting.")
    # Processes are killed atexit
    return 0


if __name__ == "__main__":
    sys.exit(main())
