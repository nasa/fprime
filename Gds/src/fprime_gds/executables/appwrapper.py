from __future__ import print_function
import sys
import time
import atexit
import signal
import pexpect

def main(args=sys.argv):
    """
    Main Program, used to wrap another program and push its results into some log file. This should be run with the
    following python command:

    python -u -m fprime_gds.executables.appwrapper

    :param args: arguments in list, or derived from sys.argv. [ stdout log, stderr log, application, [app args ...]
    :return: return code
    """
    if len(args) < 3:
        print("[ERROR] Must supply wrapper standard out, standard error, application, and optionally application args",
              file=sys.stderr)
        return 1
    # Set log arguments
    out_log = args[1]
    app_args = args[2:]
    # Write out run information
    sys.stdout.write("[INFO] Running Application: {0}\n".format(app_args))
    sys.stdout.write("[INFO] Log File: {0}\n".format(out_log))
    sys.stdout.flush()
    # Try to open file, or fail
    try:
        file_handle = open(out_log, "wb")
        atexit.register(lambda: file_handle.close())
    except IOError as ioe:
        print("[ERROR] Failed to open log file. {0}".format(ioe))
        sys.exit(1)
    # Spawn the process
    try:
        child = pexpect.spawn(app_args[0], app_args[1:], timeout=None, logfile=file_handle)

        # A cleaner exit function
        def kill_it():
            '''Kills the child'''
            child.kill(signal.SIGINT)
            time.sleep(1)
            child.kill(signal.SIGKILL)
        atexit.register(kill_it)
        while True:
            result = child.read(1)
            if sys.version_info >= (3,0):
                sys.stdout.buffer.write(result)
            else:
                sys.stdout.write(result)
            sys.stdout.flush()
    except KeyboardInterrupt:
        sys.exit(0)
    except Exception as exc:
        print("[ERROR] Failed to run application: {0}. Error: {1}".format(" ".join(app_args), exc))
        sys.exit(1)
    exit(0)


if __name__ == "__main__":
    exit(main())
