'''
Interface to the python unittest layer providing a simplified testing interface
for F' deployments.

Specifically, this provides a FPrimeTestCase which is used to allow more convenient
TestCase base class for writing FPrime unittest/integration test scripts.

Created on Aug 28, 2018

@author: mstarch
'''
from __future__ import print_function
import sys
import json
import time
import unittest
import fprime.gse.utils.gse_api
import fprime.gse.utils.test_api
import fprime.gse.testing.api_wrapper as api_wrapper
import fprime.gse.testing.support as support

class FPrimeTestCase(fprime.gse.utils.test_api.TestApi, unittest.TestCase):
    '''
    Subclass of "TestCase" used to provide F Prime based
    assertions to fprime.gse.testing users.
    '''
    __metaclass__ = support.FPrimePrettyPrint

    def __init__(self, *args, **kwargs):
        '''
        Constructs the testcase. Must be supplied a GseWrapper
        @param api: wrapped api to use for API calls
        '''
        self.carriage_return = None
        if hasattr(self, "gds_config"):
            api = api_wrapper.GdsApiWrapper.get_configured_wrapper(getattr(self, "gds_config"))
            #Call each constructor separately as classes were not designed for multi-inheritance
            fprime.gse.utils.test_api.TestApi.__init__(self, api)
        if not hasattr(self, "testbed_config"):
            self.testbed_config = {}
        unittest.TestCase.__init__(self, *args, **kwargs)
        self.verbosity = support.get_verbosity()

    def get_testbed_config(self):
        '''
        Returns the testbed configuration for this particular run. First attempts to return the
        test config for set location. If said location is not set, return default, if not set
        or no test_config supplied, then return {}.
        @return testbed_config[<supplied testbed>], or testbed_config["default"], or {}
        '''
        tb_config = support.get_testbed()
        location = tb_config["name"]
        if hasattr(self, "testbed_config"):
            try:
                ret_cfg = self.testbed_config[location]
            except KeyError:
                raise Exception("Testbed {0} does not exist in 'testbed_config' attribute"
                                .format(location))
        else:
            ret_cfg = {}
        #Add in configuration overrides
        for key, val in tb_config.iteritems():
            #Don't override name it is used above
            if key == "name":
                continue
            ret_cfg[key] = val
        return ret_cfg

    def get_config(self, key, default=None, is_bool=False):
        '''
        Gets the specified configuration key or fails the active test because it was not found
        @param key: key to return
        @param default: (optional) default value to return, if none
        @param is_bool: (optional) should the value be coerced to a boolean
        '''
        try:
            #Try to get the value, or default
            if default is None:
                tmp = self.get_testbed_config()[key]
            else:
                tmp = self.get_testbed_config().get(key, default)
            #Attempt to decode as boolean
            if is_bool:
                return self.get_bool(tmp)
            try:
                tmp = json.loads(tmp)
            except (ValueError, TypeError):
                pass
            return tmp
        except KeyError as err:
            self.fail("Config {0} not found. Use '-t <testbed>' or '-c {0} <value>' to set it"
                      .format(err))
        return None

    @staticmethod
    def get_bool(value):
        '''
        Gets a boolean from a value for use with commandline supplied items
        @param value: value to convert
        '''
        if isinstance(value, bool):
            return value
        return str(value).lower() in ["true", "yes", "y"]

    ##########
    # Helper functions for testing with pyunit, ensuring easy access
    ##########
    def print(self, *args, **kwargs):
        '''
        Local print function used to manage printing with different verbosities.
        '''
        if self.verbosity > 1 or kwargs.get("file", sys.stdout) == sys.stderr:
            if not self.carriage_return is None and self.carriage_return:
                print("", **kwargs)
                self.carriage_return = False
            print(*args, **kwargs)

    def get_latest_tlm(self, channel, timeout=5):
        '''
        Get latest telemetry with given timeout value.
        '''
        self.update() #Purges waiting TLMs
        tid = self.api.get_tlm_id(channel)
        tlm_list, _ = self.wait_tlm(channel, timeout=timeout)
        return [tlm_entry[1] for tlm_entry in tlm_list if tlm_entry[0] == tid]

    def get_latest_evr(self, evr, timeout=5, chained=False):
        '''
        Get latest telemetry with given timeout value.
        @param evr: evr to look for
        @param timeout: timeout to look for
        @param chained: (optional) will not purge wiating EVRs, as it is changed
                        to previous request
        '''
        if not chained:
            self.update() #Purges waiting EVRs
        eid = self.api.get_evr_id(evr)
        _, evr_list = self.wait_evr(evr, timeout=timeout)
        return [evr_entry[1] for evr_entry in evr_list if evr_entry[0] == eid]

    @staticmethod
    def check_tlm(tlm, value):
        '''
        Checks a telm value for correctness
        '''
        if isinstance(value, (list, tuple)):
            return tlm >= value[0] and tlm <= value[1]
        return tlm == value

    def assert_command_evr(self, cmd, evr, args=None, tokens=None, timeout=0):
        '''
        Sends a command, after set-pointing and asserts an EVR is returned
        @param cmd: command
        @param evr: evr to downlink
        @param args: (optional)arguments to send. Default: []
        @param tokens: (optional)tokens to check against EVR.
        @param timeout: (optional)timeout for waiting for EVRs
        '''
        self.update()
        #Send command, if no arguments, send empty list for arguments
        if args is None:
            args = []
        self.send(cmd, args)
        #Wait with a timeout for specific tokens
        if tokens is None:
            tokens = []
        self.assert_latest_evr(evr, tokens, timeout=timeout, chained=True)

    def assert_latest_tlm(self, channel, value, timeout=5, change=False):
        '''
        Pulls the latest EVRs, and TLM until empty. Then attempts to assert on new data.
        @param channel: TLM channel name
        @param value: either a pair of bounds, or a single exact value
        @param timeout: (optional) time to wait for the NEW tlm
        @param change: (optional) expect a change in the EVR during timeout
        '''
        try:
            start = time.time()
            message = "{0} produced no new values in {1}S".format(channel, timeout)
            tlm_vals = self.get_latest_tlm(channel, timeout)
            #Look for changes in values, by searching for some new EVR
            if change:
                message = "{0} did not change from {1} in {2}S".format(channel, tlm_vals, timeout)
                chg_vals = tlm_vals
                while set(chg_vals) == set(tlm_vals) and (time.time() - start) < timeout and \
                      not self.check_tlm(tlm_vals[-1], value):
                    remaining = max(0, timeout - (time.time() - start))
                    chg_vals = self.get_latest_tlm(channel, remaining)
                #Update tlm values to be change values and take what you get at timeout
                #or when there was a change
                #Note: if no change found, a timeout will force chg_vals to be []
                tlm_vals = chg_vals
            tlm_vals.reverse()
            self.assertGreater(len(tlm_vals), 0, message)
            for tlm_entry in tlm_vals:
                msg = "not in bounds" if isinstance(value, (list, tuple)) else "not equal to"
                self.assertTrue(self.check_tlm(tlm_entry, value),
                                "{0} of {1} {2} {3}".format(channel, tlm_entry, msg, value))
        except fprime.gse.utils.gse_api.GseApi.TimeoutException:
            self.fail("Failed to find new value for {0} in {1}S".format(channel, timeout))

    def assert_latest_evr(self, evr, tokens=[], timeout=5, chained=False):
        '''
        Pulls the latest EVRs, and TLM until empty. Then attempts to assert on new data.
        @param evr: EVR channel name
        @param tokens: EVR parameters
        @param timeout: (optional) time to wait for the NEW tlm
        @param chained: (optiona) chained to a previous assert_latest, thus do not flush incoming
        '''
        try:
            evr_vals = self.get_latest_evr(evr, timeout, chained)
            self.assertGreater(len(evr_vals), 0, "{0} was not sent in {1}S".format(evr, timeout))
            for evr_entry in evr_vals:
                #If any match, success
                if tokens is None or evr_entry == tokens:
                    break
            else:
                self.fail("{0} EVR not found with expected parameters: {1}"
                          .format(evr, tokens))
        except fprime.gse.utils.gse_api.GseApi.TimeoutException:
            self.fail("Failed to find new EVR for {0} in {1}S".format(evr, timeout))

    def assert_any_matching_evr(self, evr, tokens=[], timeout=0):
        '''
        Asserts that somewhere in the EVR history, this EVR exists
        @param evr: evr name
        @param tokens: ever parameters to check
        @param timeout: timeout values
        '''
        #Get history of this EVR
        self.update()
        evrs = self.get_evr(evr)
        if evrs is None:
            evrs = []
        for evr_tokens in evrs:
            if tokens is None or evr_tokens == tokens:
                break
        #If not found in the past, search future (chained to update call earlier)
        else:
            self.assert_latest_evr(evr, tokens, timeout=timeout, chained=True)

    def assert_monotonic_increase(self, channel, timeout=0, count=None):
        '''
        Asserts that a channel is monotonically increasing during the given timeout
        @param channel: channel to check
        @param timeout: timeout to wait for TLM
        @param count: desired number of telemetry items to receive.
                      If None supplied, then will wait full timeout and use what was found
        '''
        vals = []
        start = time.time()
        if count is None:
            count = 1000000000
        while (time.time() - start) < timeout and len(vals) < count:
            remaining = max(0, timeout - (time.time() - start))
            vals.extend(self.get_latest_tlm(channel, remaining))
        self.assertGreater(len(vals), 2, "{0} values for {1} too few to check increasing"
                           .format(len(vals), channel))
        #Check monotonically increasing
        for i in range(1, len(vals)):
            self.assertLessEqual(vals[i-1], vals[i],
                                 "{0} not monotonically increasing. {1} comes after {2}"
                                 .format(channel, vals[i], vals[i - 1]))
        self.assertLess(vals[0], vals[-1],
                        "{0} did not increase with values {1}".format(channel, vals))
