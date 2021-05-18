"""
fprime_gds.executables.utils:

Utility functions to enable the executables package to function seamlessly.
"""
import atexit
import signal
import subprocess
import time

# Python 2.7 compatibility, adding in missing error type
try:
    InterruptedError
except NameError:

    class InterruptedError(Exception):
        pass


class ProcessNotStableException(Exception):
    """ Process did not start up stably. Thus there was a problem. """

    def __init__(self, name, code, lifespan):
        """ Constructor to help with messages"""
        super().__init__(
            "{} stopped with code {} sooner than {} seconds".format(
                name, code, lifespan
            )
        )


class AppWrapperException(Exception):
    """
    An exception occurred while tying to start the app wrapper. This will encapsulate that message.
    """


def register_process_assassin(process, log=None):
    """
    Register an assassin that will kill the a given child process when an exit of the current python process has been
    reached. This will effectively clean up children and (optionally) their log files.

    :param process: the process to kill.
    :param log: a paired log file to kill as well.
    """

    def assassin():
        """
        Kill process and ensure that it is really really dead.
        Note: enclosing the locals in the closure, allows this function to operate independently.
        """
        # First attempt to kill the process uses SIGINT/SIGTERM giving the process a bit to wrap up its affairs.
        # This code allows for both pexpect and subprocess processes.
        try:
            if hasattr(process, "terminate"):
                process.terminate()
            else:
                process.kill(signal.SIGINT)
            time.sleep(1)
        except (KeyboardInterrupt, OSError, InterruptedError):
            pass
        # Second attempt is to terminate with extreme prejudice. No process will survive this, ensuring that it is
        # really, really dead. Supports both pexpect and subprocess.
        try:
            if hasattr(process, "terminate"):
                process.kill()
            else:
                process.kill(signal.SIGKILL)
        except (KeyboardInterrupt, OSError, InterruptedError):
            pass
        # Might as well close the log file because dead men tell no tales.
        try:
            if log is not None:
                log.close()
        except (KeyboardInterrupt, OSError, InterruptedError):
            pass

    atexit.register(assassin)


def run_wrapped_application(arguments, logfile=None, env=None, launch_time=None):
    """
    Run an application and ensure that it is logged immediately to the logfile. This will allow the application to have
    up-to-date logs. This is a wrapper for pexpect to ensure that the application runs and log effectively. It has been
    converted to a function to remove superfluous processes.

    :param arguments: arguments with the first being the executable.
    :param logfile: (optional) path to logfile to log to. Will overwrite.
    :param env: (optional) environment for the subprocess
    :param launch_time: (optional) time to wait before declaring the process stable
    :return: child process should it be needed.
    """
    # Write out run information for the calling user
    print("[INFO] Running Application: {}".format(arguments[0]))
    # Attempt to open a log file
    file_handler = None
    try:
        if logfile is not None:
            print("[INFO] Log File: {}".format(logfile))
            file_handler = open(logfile, "wb", 0)
    except OSError as exc:
        raise AppWrapperException(
            "Failed to open: {} with error {}.".format(logfile, str(exc))
        )
    # Spawn the process. Uses pexpect, as this will force the process to output data immediately, rather than buffering
    # the output. That way the log file is fully up-to-date.
    try:
        child = subprocess.Popen(
            arguments, stdout=file_handler, stderr=subprocess.STDOUT, env=env
        )
        register_process_assassin(child, file_handler)
        # If launch time is specified, then wait for it to be stable
        if launch_time is not None:
            time.sleep(launch_time)
            child.poll()
            if child.returncode is not None:
                raise ProcessNotStableException(
                    arguments[0], child.returncode, launch_time
                )
        return child
    except Exception as exc:
        raise AppWrapperException(
            "Failed to run application: {}. Error: {}".format(" ".join(arguments), exc)
        )
