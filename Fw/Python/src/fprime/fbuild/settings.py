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
    def find_fprime():
         """
         Finds F prime by recursing parent to parent until a matching directory is found.
         """
         needle = os.path.join("cmake", "FPrime.cmake") # If the F prime cmake file exists
         path = os.getcwd()
         while path != os.path.dirname(path):
              if os.path.isfile(os.path.join(path, needle)):
                  return os.path.abspath(os.path.normpath(path))
              path = os.path.dirname(path)
         raise FprimeLocationUnknownException("Please set 'fprime_location' in [fprime] section in 'settings.ini")


    @staticmethod
    def read_safe_path(parser, section, key, ini_file):
        """
        Reads path(s), safely, from the config parser.  Validates the path(s) exists or raises an exception. Paths are
        separated by ':'.  This will also expand relative paths relative to the settings file.
        :param parser: parser to read from
        :param key: key to read from
        :param ini_file: ini_file path for clean errors
        :return path, validated
        """
        base_dir = os.path.dirname(ini_file)
        all_paths = parser.get(section, key, fallback="").split(":")
        expanded = []
        for path in all_paths:
            if path == "" or path is None:
                continue
            full_path = os.path.abspath(os.path.normpath(os.path.join(base_dir, path)))
            if not os.path.exists(full_path):
                raise FprimeSettingsException("Non-existant path '{}' found in section '{}' option '{}' of file '{}'"
                                              .format(path, section, key, ini_file))
            expanded.append(full_path)
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
            fprime_location = IniSettings.find_fprime()
            return {
                 "fprime_location": fprime_location,
                 "environment": {
                     "FPRIME_LOCATION": fprime_location
                 }
            }
        confparse = configparser.ConfigParser()
        confparse.read(settings_file)
        # Search through F prime locations
        fprime_location = IniSettings.read_safe_path(confparse, "fprime", "fprime_location", settings_file)
        if not fprime_location:
            fprime_location = IniSettings.find_fprime()
        else:
            fprime_location = fprime_location[0]
        # Read separate environment file if necessary
        env_file = IniSettings.read_safe_path(confparse, "fprime", "environment_file", settings_file)
        env_file = None if not env_file else env_file[0]
        libraries = IniSettings.read_safe_path(confparse, "fprime", "fprime_library_locations", settings_file)
        if env_file is not None:
            env_parser = configparser.ConfigParser()
            env_parser.read(env_file)
        else:
            env_parser = confparse
        environment = IniSettings.load_environment(env_parser)
        # Force some environment settings
        if env_file is not None:
            environment["FPRIME_ENVIRONMENT_FILE"] = env_file
        environment["FPRIME_SETTINGS_FILE"] = settings_file
        environment["FPRIME_LOCATION"] = fprime_location
        if libraries:
            environment["FPRIME_LIBRARY_LOCATIONS"] = ";".join(libraries)
        return {
            "fprime_location": fprime_location,
            "fprime_library_locations": libraries,
            "default_toolchain": confparse.get("fprime", "default_toolchain", fallback="native"),
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


class FprimeLocationUnknownException(Exception):
    """ Fprime location could not be determined """
    pass


class FprimeSettingsException(Exception):
    """ An exception for handling F prime settings misconfiguration """
    pass
