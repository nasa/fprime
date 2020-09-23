"""
__init__.py:

Python Logging Setup. This sets up the global logging format for all Gds components. This allows the user to use Python
loggers without calling basic config.

@author mstarch
"""
import logging
import os
import sys


def configure_py_log(directory=None, filename=sys.argv[0], mode="w"):
    """
    Configure the python logging. If logdir is supplied, our logs will go in that directory as a log file. Otherwise,
    logs will go to the CLI.

    :param filename: logging filename
    :param logdir: directory to log file into
    :param mode: of file to write
    """
    if directory is None:
        logging.basicConfig(
            level=logging.INFO,
            format="[%(asctime)s] [%(levelname)s] %(name)s: %(message)s",
        )
    else:
        logging.basicConfig(
            filename=os.path.join(directory, filename),
            filemode=mode,
            level=logging.INFO,
            format="[%(asctime)s] [%(levelname)s] %(name)s: %(message)s",
        )
