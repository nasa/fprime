"""
flask/components.py:

This sets up the primary data components that allow Flask to connect into the system. This is where the standard
pipeline and other components are created to interact with Flask.
"""
import os

import fprime_gds.common.pipeline.standard

# Module variables, should remain hidden. These are singleton top-level objects used by Flask, and its various
# blueprints needed to run the system.
__PIPELINE = None


def setup_pipelined_components(
    debug, logger, config, dictionary, down_store, log_dir, tts_address, tts_port
):
    """
    Setup the standard pipeline and related components. This is done once, and then the resulting singletons are
    returned so that one object is used throughout the system.

    :param debug: used to prevent the construction of the standard pipeline
    :param logger: logger to use for output
    :param config: GDS configuration
    :param dictionary: path to F prime dictionary
    :param down_store:
    :param log_dir: log directory to write logs to, and serve logs from
    :param tts_address: address to the middleware layer
    :param tts_port: port of the middleware layer
    :return: F prime pipeline
    """
    global __PIPELINE
    if (
        __PIPELINE is None
        and not debug
        or os.environ.get("WERKZEUG_RUN_MAIN") == "true"
    ):
        pipeline = fprime_gds.common.pipeline.standard.StandardPipeline()
        pipeline.setup(config, dictionary, down_store, logging_prefix=log_dir)
        logger.info(
            "Connecting to GDS at: {}:{} from pid: {}".format(
                tts_address, tts_port, os.getpid()
            )
        )
        pipeline.connect(tts_address, tts_port)
        __PIPELINE = pipeline
    return __PIPELINE


def get_pipelined_components():
    """
    Returns the setup pipelined components, or raises exception if not setup yet.

    :return: F prime pipeline
    """
    assert __PIPELINE is not None, "Pipeline must be setup before use"
    return __PIPELINE
