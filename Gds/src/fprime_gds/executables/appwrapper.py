from __future__ import print_function
import sys
import time
import atexit
import subprocess
import threading


class LoggingWrapper(object):
    """
    Logging wrapper that pretends to be a file-like object, allowing to both print the output and log it to an existing
    file. This essentially tee's in an OS independent way.
    """
    def __init__(self, sysio, file_path):
        """
        Initialize the wrapper object with file_path.
        :param sysio: system io stream for writing to
        :param file_path: path to open and write to
        """
        self.stop = False
        self.watcher = None
        self.fn = file_path
        self.fh = open(file_path, "wb+")
        self.sysio = sysio

    def write(self, data):
        """
        Write data handler
        :param data: data to write
        """
        self.fh.write(data)
        self.sysio.write(data)
        self.fh.flush()
        self.sysio.flush()

    def set_watcher(self, thread):
        """
        Set the watcher for this logger
        :param thread: watcher
        """
        self.watcher = thread

    def close(self):
        """
        Close out the file handler
        """
        # Wait for thread death
        self.stop = True
        if self.watcher is not None:
            self.watcher.join()
        self.fh.close()


def watcher(stream, wrapper):
    """
    Thread target that reads and dumps a PIPE output from subprocess
    :param stream: stream to read
    :param wrapper: log wrapper to write to
    """
    output = None
    while output != b'' and not wrapper.stop:
        output = stream.read(1)
        wrapper.write(output)


def main(args=sys.argv):
    """
    Main Program, used to wrap another program and push its results into some log file. This should be run with the
    following python command:

    python -u -m fprime_gds.executables.appwrapper

    :param args: arguments in list, or derived from sys.argv. [ stdout log, stderr log, application, [app args ...]
    :return: return code
    """
    if len(args) < 4:
        print("[ERROR] Must supply wrapper standard out, standard error, application, and optionally application args",
              file=sys.stderr)
        return 1
    # Set log arguments
    out_log = args[1]
    err_log = args[2]
    try:
        if out_log == err_log:
            stderr = subprocess.STDOUT
        else:
            stderr = LoggingWrapper(sys.stderr, err_log)
            atexit.register(lambda: stderr.close())
        stdout = LoggingWrapper(sys.stdout, out_log)
        atexit.register(lambda: stdout.close())
    except IOError as ioe:
        print("[ERROR] Failed to create logging wrapper. {0}".format(ioe), file=sys.stderr)
        exit(2)
    # Application arguments moving forward
    app_args = args[3:]
    # Write out run information
    stdout.write("[INFO] Running Application: {0}\n".format(app_args))
    stdout.write("[INFO] STDOUT Log: {0}\n".format(stdout.fn))
    if stderr != subprocess.STDOUT:
        stdout.write("[INFO] STDERR Log: {0}\n".format(stderr.fn))
        stderr.write("[INFO] Running Application: {0}\n".format(app_args))
        stderr.write("[INFO] STDOUT Log: {0}\n".format(stdout.fn))
        stderr.write("[INFO] STDERR Log: {0}\n".format(stderr.fn))
    threads = []
    try:
        # Start a process and wait for it to die
        proc = subprocess.Popen(app_args, bufsize=0, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE if type(stderr) == LoggingWrapper else stderr)
        atexit.register(lambda: proc.kill())
        # Start a stdout thread
        thread = threading.Thread(target=watcher, args=(proc.stdout, stdout))
        thread.daemon = True
        stdout.set_watcher(thread)
        threads.append(thread)
        # Start a stderr thread if needed
        if stderr != subprocess.STDOUT:
            thread = threading.Thread(target=watcher, args=(proc.stderr, stderr))
            thread.daemon = True
            stderr.set_watcher(thread)
            threads.append(thread)
        # Start threads in daemon mode
        for thread in threads:
            thread.start()
        # Spin forever
        while True:
            time.sleep(500)
    except KeyboardInterrupt:
        exit(0)


if __name__ == "__main__":
    exit(main())
