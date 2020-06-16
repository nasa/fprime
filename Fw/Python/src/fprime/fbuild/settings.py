"""
fprime.fbuild.settings:

An implementation used to pull settings into the fprime build.  This version uses INI files in order to load the
settings from the settings.default file that is part of the F prime deployment directory.

@author mstarch
"""
import os
import configparser # Written after PY2 eol


class IniSettings():
    """Class to load settings from INI files"""
    DEF_FILE = "settings.ini"
    SET_ENV = "FPRIME_SETTINGS_FILE"

    @staticmethod
    def read_safe_path(parser, section, key, ini_file, allow_none=False):
        """
        Reads path(s), safely, from the config parser.  Validates the path(s) exists or raises an exception. Paths are
        separated by ':'.  This will also expand relative paths relative to the settings file.
        :param parser: parser to read from
        :param key: key to read from
        :param ini_file: ini_file path for clean errors
        :param allow_none: (optional) allows None values as valid.  Default: False
        :return path, validated
        """
        base_dir = os.path.dirname(ini_file)
        try:
            if allow_none:
                paths = parser.get(section, key, fallback="")
            else:
                paths = parser.get(section, key)
        except (configparser.NoSectionError, configparser.NoOptionError, KeyError):
            raise FprimeSettingsException("Failed to find required key '{}' in section'{}' of file '{}'"
                                          .format(key, section, ini_file))
        expanded = []
        all_paths = paths.split(":")
        if all_paths == "" and allow_none:
            return [None]
        for path in all_paths:
            if allow_none and (path == "" or path is None):
                continue
            elif not os.path.exists(path):
                raise FprimeSettingsException("Non-existant path '{}' found in section '{}' option '{}' of file '{}'"
                                              .format(path, section, key, ini_file))
            expanded.append(os.path.abspath(os.path.join(base_dir, path)))
        return expanded


    @staticmethod
    def load(settings_file):
        """
        Load settings from specified file or from specified build directory. Either a specific file or the build
        directory must be not None.
        :param settings_file: (optional) file to load settings from (in INI format). Must be specified if build_dir is not.
        :param build_dir: (optional) directory to search for settings.ini. Must be specified if file is not.
        :return: a dictionary of needed settings
        """
        settings_file = settings_file if settings_file is not None else os.path.join(IniSettings.DEF_FILE)
        # Check file existence if specified
        if not os.path.exists(settings_file):
            print("[WARNING] Failed to find settings file: {}".format(settings_file))
            return {}
        confparse = configparser.ConfigParser()
        confparse.read(settings_file)
        # Read separate environment file if necessary
        env_file = IniSettings.read_safe_path(confparse, "fprime", "build_environment_settings", settings_file, True)
        env_file = None if not env_file else env_file[0]
        toolchains = IniSettings.read_safe_path(confparse, "fprime", "toolchain_locations", settings_file, True)
        if env_file is not None:
            env_parser = configparser.ConfigParser()
            env_parser.read(env_file)
        else:
            env_parser = confparse
        environment = IniSettings.load_environment(env_parser)
        environment["FPRIME_SETTINGS_FILE"] = settings_file
        if env_file is not None:
            environment["FPRIME_ENVIRONMENT_FILE"] = env_file
        return {
            "fprime_location": confparse.get("fprime", "fprime_location", fallback=None),
            "default_toolchain": confparse.get("fprime", "default_toolchain", fallback=None),
            "toolchain_locations": toolchains,
            "environment": environment
        }

    @staticmethod
    def load_environment(parser):
        """
        Load the environment from the given parser.
        :param parser: parser to load environment from
        :return: environment dictionary
        """
        env_dict = {}
        try:
            for key, value in parser.items("environment"):
                env_dict[key] = value
        except configparser.NoSectionError:
            pass # Ignore missing environment
        return env_dict


class FprimeSettingsException(Exception):
    """ An exception for handling F prime settings misconfiguration """
    pass