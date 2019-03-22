#
#===============================================================================
# NAME: ConfigManager.py
#
# DESCRIPTION: This is a simple configuration class patterned after the
#		one in the Keck observation sequencer GUI and the Tahoe
#       CalVal pipeline.  The configuration class has changed since
#       previous ones.  This new one uses the ConfigParser module
#       included with Python 2.4 to extend configuration out to
#       reading windows like .ini files.  If none exist then this
#       uses hardwired values set in a dictionary called prop to
#       default initiallization.
#
# AUTHOR: Leonard J. Reder
#
# EMAIL:  reder@jpl.nasa.gov
#
# DATE CREATED  : 30 January 2007
#
# Copyright 2007, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
import os
import sys
import glob
import ConfigParser

from fprime_gds.common.models.common.event import Severity
import tkGui_misc

class ConfigManager(ConfigParser.SafeConfigParser):
    """
    This class provides a single entrypoint for all configurable properties,
    namely the self.Prop dictionary.
    """

    __instance = None
    __prop     = None

    def __init__(self):
        """
	    Constructor.
	    """
        ConfigParser.SafeConfigParser.__init__(self)
        ConfigParser.ConfigParser.__init__(self)
        ConfigParser.RawConfigParser.__init__(self)
        self.__prop   = dict()
        self._setProps()

        # Look for an gse.ini file.
        # The order ConfigManager reads the files is
        # determined by the order the files are passed to read().
        # The last configuration file read will be the selected
        # configuration.
        # If a gse.ini file is not found then the default
        # values from setProps are used.
        config_file_name = 'gse.ini'
        files = list()

        # Look for gse.ini in $HOME/fprime_logs...
        files.append(os.environ['HOME'] + os.sep + 'fprime_logs' + os.sep + config_file_name)
        # In BUILD_ROOT/Gse/generated...
        if 'BUILD_ROOT' in os.environ:
            files.append(os.environ['BUILD_ROOT'] + os.sep + 'Gse' + os.sep + 'generated' + os.sep + config_file_name)
        # And in current directory
        files.append(os.getcwd() + os.sep + config_file_name)

        self.read(files)


    def getInstance():
        """
        Return instance of singleton.
        """
        if(ConfigManager.__instance is None):
            ConfigManager.__instance = ConfigManager()
        return ConfigManager.__instance

    #define static method
    getInstance = staticmethod(getInstance)


    def _setProps(self):
        """
	    Used only by constructor to set all ConfigParser defaults. Establishes
        a dictionary of sections and then a dictionary of keyword, value
        association for each section.
	    @params None
        """


        ################################################################
        # Default file paths here.
        ################################################################
        self.__prop['filepaths'] = dict()

        # Setup default for generated python command / telemetry descriptor files.
        # These are autocoded descriptions of all.
        #
        # Look for BUILD_ROOT and if not set look for generated in "." and then
        # in ".." and if not found then throw an exception.
        try:
            build_root = os.environ['BUILD_ROOT']
        except KeyError, e:
            print "Config Manager BUILD_ROOT not found."
            build_root = ''

        generated_path = os.path.join(build_root, 'Gse/generated')

        self.__prop['filepaths']['build_root']     = build_root
        self.__prop['filepaths']['generated_path'] = generated_path
        #
        # Sequence file save path
        # Default: Current Directory
        self.__prop['filepaths']['sequence_save_path'] = os.getcwd() + os.sep + "fprime" + os.sep + "sequences"
        #
        # Log file save path
        # Default: Current Directory
        self.__prop['filepaths']['log_file_path'] = os.environ['HOME'] + os.sep + "fprime_logs"



        ################################################################
        # Default GUI parameters here.
        ################################################################

        self.__prop['tables'] = dict()
        self.__prop['tables']['font'] = 'System'
        self.__prop['tables']['font_size'] = 12

        self.__prop['gui'] = dict()
        self.__prop['gui']['window_min_x']    = 700
        self.__prop['gui']['window_min_y']    = 600
        self.__prop['gui']['window_offset_x'] = 10
        self.__prop['gui']['window_offset_y'] = 10

        self.__prop['performance'] = dict()
        self.__prop['performance']['stripchart_update_period']  = 500 # Milliseconds
        self.__prop['performance']['telem_table_update_period'] = 500 # Milliseconds
        self.__prop['performance']['event_table_update_period'] = 200 # Milliseconds
        self.__prop['performance']['status_bar_update_period']  = 1000 # Milliseconds
        self.__prop['performance']['stripchart_update_period']  = 500 # Milliseconds
        self.__prop['performance']['status_light_deadband'] = 3 # Seconds

        self.__prop['severity_colors']=dict()
        self.__prop['severity_colors'][Severity.COMMAND.name]    = "lightblue"
        self.__prop['severity_colors'][Severity.DIAGNOSTIC.name] = ""
        self.__prop['severity_colors'][Severity.ACTIVITY_HI.name]= ""
        self.__prop['severity_colors'][Severity.ACTIVITY_LO.name]= ""
        self.__prop['severity_colors'][Severity.WARNING_LO.name] = "yellow"
        self.__prop['severity_colors'][Severity.WARNING_HI.name] = "yellow"
        self.__prop['severity_colors'][Severity.FATAL.name]      = "red"

        self.__prop['about_info']=dict()
        self.__prop['about_info']['version_number'] = 'x.x'
        self.__prop['about_info']['version_date']   = 'x/x/x'
        self.__prop['about_info']['last_modified']  = 'Last Modified: x/x/x'
        self.__prop['about_info']['copyright']      = 'Copyright: 2016 California Institute of Technology. \nAll rights reserved.'
        self.__prop['about_info']['contact']        = ''

        str1 = 'F Prime, Ground Support Equipment (GSE)'
        str2 = 'NASA Jet Propulsion Laboratory, California Institute of Technology'
        self.__prop['about_info']['app_name']       = str1 + '\n' + str2
        #

        self.__prop['persist'] = dict()
        self.__prop['persist']['save_file'] = os.path.join(build_root, "Gds/configs/gse_persist.pkl")

        self.__prop['strippanel'] = dict()
        self.__prop['strippanel']['plot_height'] = 3 #Height in Inches
        self.__prop['strippanel']['default_timewindow'] = 10 # Time window in seconds
        self.__prop['strippanel']['default_tick_amount'] = 10
        self.__prop['strippanel']['plot_update_rate'] = 1
        self.__prop['strippanel']['max_columns']  = 2
        self.__prop['strippanel']['buffer_capacity'] = 500 # Number of data points held
        self.__prop['strippanel']['plot_left_margin']   = 0.08
        self.__prop['strippanel']['plot_bottom_margin'] = 0.4

        self.__prop['channel_max_limit'] = dict()
        self.__prop['channel_max_limit']['Sensor1'] = 200

        self.__prop['channel_min_limit'] = dict()
        self.__prop['channel_min_limit']['Sensor1'] = 10

        self.__prop['file_management'] = dict()
        self.__prop['file_management']['default_uplink_path'] = ''
        self.__prop['file_management']['default_downlink_path'] = os.path.join(os.environ['HOME'], 'fprime')

        self.__prop['helppanel'] = dict()
        self.__prop['helppanel']['doc_folder'] = os.path.join(build_root, "Gds/src/fprime_gds/tkgui/docs")
        self.__prop['helppanel']['max_header_size'] = 36
        self.__prop['helppanel']['min_header_size'] = 18
        self.__prop['helppanel']['default_font_size'] = 14
        self.__prop['helppanel']['default_header_link_size'] = 16
        self.__prop['helppanel']['panel_header_font_size'] = 18


        # 'DEFAULT' section is baked in for Python 2.6x
        # the call below will raise ValueError
        #

        # This sets the defaults within a section.
        # self._setSectionDefaults('DEFAULT')

        #

        self._setSectionDefaults('persist')
        self._setSectionDefaults('helppanel')
        self._setSectionDefaults('performance')
        self._setSectionDefaults('channel_min_limit')
        self._setSectionDefaults('channel_max_limit')
        self._setSectionDefaults('file_management')
        self._setSectionDefaults('tables')
        self._setSectionDefaults('filepaths')
        self._setSectionDefaults('gui')
        self._setSectionDefaults('severity_colors')
        self._setSectionDefaults('about_info')
        self._setSectionDefaults('strippanel')


    def _setSectionDefaults(self, section):
        """
        For a section set up the default values.
        """
        self.add_section(section)
        for (key,value) in self.__prop[section].items():
            self.set(section, key, "%s" % value)


if __name__ == '__main__':
    #
    # Quick test of configure defaults.
    #
    config = ConfigManager().getInstance()
    print
    print 'IPC section defaults:'
    for (key, value) in config.items('ipc'):
        print "%s = %s" % (key, value)
    print
    print 'Get some of the ipc values:'
    print 'h_pub_suffix = %s' % config.get('ipc','h_pub_suffix')
    print 'h_msg_suffix = %s' % config.get('ipc','h_msg_suffix')
    print 'c_int_suffix = %s' % config.get('ipc','c_int_suffix')
    print 'c_dispatch_suffix = %s' % config.get('ipc','c_dispatch_suffix')
    print 'c_cmd_dispatch_suffix = %s' % config.get('ipc', 'c_cmd_dispatch_suffix')
