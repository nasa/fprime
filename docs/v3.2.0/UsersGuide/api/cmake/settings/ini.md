**Note:** auto-generated from comments in: ./settings/ini.cmake

## ini.cmake:

This file loads settings from INI files. In cases where these settings are supplied via fprime-util, the settings are
checked for discrepancy. This allows the cmake system to detect when it should be regenerated and has not been. In the
case that the settings were not supplied, it sets them.


## FPRIME_UTIL_CRITICAL_LIST:

This is a list of critical settings that are passed in from fprime-util. If these settings change in `settings.ini`
they need to result in a WARNING to let the user know that had fprime-util be run, it now needs to be rerun.


## ini_to_cache:

Uses a python script to load INI files. These items are set into the CMake cache.


