####
# default_settings.py:
#
# Contains default setup for the F prime flask application. Specifically, it is used to pass configuration
# down to the GDS config layers, and is used to specify a dictionary and packet spec for specifying
# the event, channels, and commands setup.
#
# Note: flask configuration is all done via Python files 
#
####
import os
import fprime_gds.common.utils.config_manager

# Get dictionary from environment
DICTIONARY = os.environ.get("DICTIONARY", None)
PORT = int(os.environ.get("TTS_PORT", "50050"), 0)
ADDRESS = os.environ.get("TTS_ADDR", "0.0.0.0")
LOG_DIR = os.environ.get("LOG_DIR", None)

# Gds config setup
GDS_CONFIG = fprime_gds.common.utils.config_manager.ConfigManager()
#TODO: load real config
