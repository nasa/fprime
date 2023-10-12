#
# ===============================================================================
# NAME: Logger.py
#
# DESCRIPTION: Logger module sets up the logging for all the other
#              scripts based on the Python logging module.  It does
#              not have a main routine and is not intended to
#              execute standalone.
#
#              If more loggers are needed they can be created here.
#
# PUBLIC FUNCTIONS:
#   connectOutputLogger(file=None)
#   connectDebugLogger(level=logging.INFO,file=None, stdout_enable=False)
#
# AUTHOR: Leonard J. Reder
#         Simmon J. Hook
#
# EMAIL:  reder@jpl.nasa.gov
#
# DATE CREATED: 26 September 2006
#
# COMMENT:
#
# Copyright 2006, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging
import sys

#
# Universal globals used within module
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
# Two public functions for configuring the loggers.
# @todo: Move these to an independent module.
#
def connectOutputLogger(file=None, logger_name="output"):
    """
    Function that connects the output logger.
    This is always enabled and hardwired to
    generate logging.INFO level messages only.
    @param   file : Output file to store these messages into.
    @default None : If file is None no output file generated.
    """
    # Create logger for logging messages
    logger = logging.getLogger(logger_name)
    #
    # Define and set the format of the logged messages
    #
    formatter = logging.Formatter("%(message)s")
    #
    # Create handlers here
    #
    std_output = logging.StreamHandler(sys.stdout)
    std_output.setFormatter(formatter)
    logger.addHandler(std_output)
    # log to a file
    if file is not None:
        file_output = logging.FileHandler(file)
        file_output.setFormatter(formatter)
        logger.addHandler(file_output)
    logger.setLevel(logging.INFO)
    return logger


def connectDebugLogger(
    level=logging.INFO, file=None, stdout_enable=False, logger_name="debug"
):
    """
    Function that connects the logger for debugging.
    This is enabled by the initial user call.  The user is free to use all
    levels of logging following this convention.
    @param      level           : initial level of logging for debugging.
    @default    logging.INFO    :
    @param      file            : file name and path for debug log.
    @default    None            : file is None so nothing generated.
    @param      stdout_enable   : Enables output to sys.stdout if True.
    @default    False
    """
    # Create logger for logging messages
    logger = logging.getLogger(logger_name)
    #
    # Define and set the format of the logged messages
    #
    formatter = logging.Formatter("%(levelname)s %(lineno)d %(message)s")
    #
    # Create handlers here
    #
    # stdout for testing
    if stdout_enable:
        std_output = logging.StreamHandler(sys.stdout)
        std_output.setFormatter(formatter)
        logger.addHandler(std_output)
    # log to a file
    if file is not None:
        file_output = logging.FileHandler(file)
        file_output.setFormatter(formatter)
        logger.addHandler(file_output)
    #
    # Set the logging level here.
    #
    if level is None:
        level = 0
    logger.setLevel(level)
    logger.info("Log file set to: %s" % file)
    logger.info("Logging level is %s" % level)
    logger.info("%s author: Leonard J. Reder" % sys.argv[0])
    logger.info("Email: reder@jpl.nasa.gov")
    return logger
