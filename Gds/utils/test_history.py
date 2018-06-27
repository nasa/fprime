#!/bin/env python
#===============================================================================
# NAME: test_history.py
#
# DESCRIPTION: A basic test framework for integration testing.
# AUTHOR: Kevin Dinkel
# EMAIL:  dinkel@jpl.nasa.gov
# DATE CREATED: November 19, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
from utils.history import TelemetryHistory
from utils.history import EventHistory
import utils.history as history

__author__ = "Kevin Dinkel"
__copyright__ = "Copyright 2015, California Institute of Technology."
__version__ = "1.0"
__email__ = "kevin.dinkel@jpl.nasa.gov"

class TestHistory(object):
  '''
  A test history interface tailored for use with events and telemetry.
  '''
  ###############################
  # Private class variables:
  ###############################
  
  def __init__(self):
    '''
    A set of dictionaries for telemetry and events:
      key: the evr or tlm id
      value: a list of the evr or tlm args associated with each received
    '''
    self._evr_hist = EventHistory()
    self._tlm_hist = TelemetryHistory()

    '''
    This is an object which specifies that all elements should be checked during a comparison.
    It can be used as an input to index as follows:
    
    # Ensure that all elements of type "foo" equal 1.
    TestApi.assert_eq(1, "foo", index=TestApi.ALL)
    '''
    self.ALL = history.ALL
    
    '''
    This is an object which specifies that any of elements in the dictionary can satisfy a 
    comparison. It can be used as an input to index as follows:
    
    # If any element of "foo" equals 1, then this assertion will pass.
    TestApi.assert_eq(1, "foo", index=TestApi.ANY)
    '''
    self.ANY = history.ANY
    
    '''
    This is an object which always returns True during a comparison. This can be used when
    running assertions. For example, say an event returns an argument array [foo, boo, you].
    but you only cate to assert against the first two elements, and do not care what the third
    element is. You can achieve this by calling the method:
    
    TestApi.assert_evr([foo, boo, TestApi.ANYTHING])
    '''
    self.ANYTHING = history.ANYTHING
    
    '''
    This is an object that helps in comparing floating point numbers. Creating an object of
    type 'NEAR' will compare floating point values to within some epsilon. Here is an example.
    
    TestApi.assert_tlm(TestApi.NEAR(3.1415, epsilon=0.01))
    '''
    self.NEAR = history.NEAR

    super(TestHistory, self).__init__()

  ###############################
  # Public API methods:
  ###############################

  ###################################################################################
  ###################################################################################
  ## Populate histories:
  ###################################################################################
  ###################################################################################
  def add_evr(self, evr_list):
    ''' 
    Add events to history
    @param evr_list: a list of events in the format
      [(event_name,[list, of, event, args]), (event_name2, ...etc.]
    '''
    self._evr_hist.add(evr_list)

  def add_tlm(self, tlm_list):
    ''' 
    Add telemetry to history
    @param tlm_list: a list of telemetry in the format
      [(tlm_name,tlm_val), (tlm_name2, ...etc.]
    '''
    self._tlm_hist.add(tlm_list)

  def add(self, evr_list, tlm_list):
    ''' 
    Add events and telemetry to history
    @param evr_list: a list of events in the format
      [(event_name,[list, of, event, args]), (event_name2, ...etc.]
    @param tlm_list: a list of telemetry in the format
      [(tlm_name,tlm_val), (tlm_name2, ...etc.]
    '''
    self._evr_hist.add(evr_list)
    self._tlm_hist.add(tlm_list)

  ###################################################################################
  ###################################################################################
  ## Clear histories:
  ###################################################################################
  ###################################################################################
  def clear_evr(self):
    ''' 
    Remove all events from the event history
    '''
    self._evr_hist.clear()

  def clear_tlm(self):
    ''' 
    Remove all telemetry from the telemetry history
    '''
    self._tlm_hist.clear()

  def clear(self):
    ''' 
    Remove all events from the event history and remove all telemetry from the telemetry history
    '''
    self.clear_evr()
    self.clear_tlm()

  ###################################################################################
  ###################################################################################
  ## Print helpers:
  ###################################################################################
  ###################################################################################
  def print_evr(self):
    ''' 
    Print the contents of the event history dictionary
    '''
    return self._evr_hist.get_hist_str()

  def print_tlm(self):
    ''' 
    Return the contents of the telemetry history dictionary
    '''
    return self._tlm_hist.get_hist_str()

  def get_pretty_print(self):
    ''' 
    Return the contents of the telemetry and event history dictionaries
    '''
    ret_str = ""
    ret_str += "---------------------\n"
    ret_str += "EVR:\n"
    ret_str += "---------------------"
    ret_str += self.print_evr()
    ret_str += "\n\n---------------------\n"
    ret_str += "TLM:\n"
    ret_str += "---------------------"
    ret_str += self.print_tlm()
    ret_str += "\n---------------------\n"
    return ret_str

  def pretty_print(self):
    '''
    Print the contents of the telemetry and event history dictionaries
    '''
    print self.get_pretty_print()

  ###################################################################################
  ###################################################################################
  ## Test event size:
  ###################################################################################
  ###################################################################################
  def assert_evr_size(self, size, evr_name=None, filterFunc=None):
    '''
    Assert the number of events received is equal to 'size'.
    @param size: the number of events expected
    @param evr_name: (optional) if provided, only check the size of events of
    this type
    @param filterFunc: (optional) if provided, only events arguments that return true when passed
    into this function are counted. For example, to only count event arguments with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._evr_hist.assert_size(size, evr_name, filterFunc)

  def assert_evr_size_eq(self, size, evr_name=None, filterFunc=None): # Same as above, but here for convenience
    '''
    Assert the number of events received is equal to 'size'.
    @param size: the number of events expected
    @param evr_name: (optional) if provided, only check the size of events of
    this type
    @param filterFunc: (optional) if provided, only events arguments that return true when passed
    into this function are counted. For example, to only count event arguments with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._evr_hist.assert_size_eq(size, evr_name, filterFunc)

  def assert_evr_size_ne(self, size, evr_name=None, filterFunc=None):
    '''
    Assert the number of events received is not equal to 'size'.
    @param size: the number of events expected
    @param evr_name: (optional) if provided, only check the size of events of
    this type
    @param filterFunc: (optional) if provided, only events arguments that return true when passed
    into this function are counted. For example, to only count event arguments with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._evr_hist.assert_size_ne(size, evr_name, filterFunc)

  def assert_evr_size_lt(self, size, evr_name=None, filterFunc=None):
    '''
    Assert the number of events received is less than 'size'.
    @param size: the number of events expected
    @param evr_name: (optional) if provided, only check the size of events of
    this type
    @param filterFunc: (optional) if provided, only events arguments that return true when passed
    into this function are counted. For example, to only count event arguments with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._evr_hist.assert_size_lt(size, evr_name, filterFunc)
    
  def assert_evr_size_le(self, size, evr_name=None, filterFunc=None):
    '''
    Assert the number of events received is less than or equal to 'size'.
    @param size: the number of events expected
    @param evr_name: (optional) if provided, only check the size of events of
    this type
    @param filterFunc: (optional) if provided, only events arguments that return true when passed
    into this function are counted. For example, to only count event arguments with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._evr_hist.assert_size_le(size, evr_name, filterFunc)

  def assert_evr_size_gt(self, size, evr_name=None, filterFunc=None):
    '''
    Assert the number of events received is greater than 'size'.
    @param size: the number of events expected
    @param evr_name: (optional) if provided, only check the size of events of
    this type
    @param filterFunc: (optional) if provided, only events arguments that return true when passed
    into this function are counted. For example, to only count event arguments with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._evr_hist.assert_size_gt(size, evr_name, filterFunc)

  def assert_evr_size_ge(self, size, evr_name=None, filterFunc=None):
    '''
    Assert the number of events received is greater than or equal to 'size'.
    @param size: the number of events expected
    @param evr_name: (optional) if provided, only check the size of events of
    this type
    @param filterFunc: (optional) if provided, only events arguments that return true when passed
    into this function are counted. For example, to only count event arguments with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._evr_hist.assert_size_ge(size, evr_name, filterFunc)

  ###################################################################################
  ###################################################################################
  ## Test telemetry size:
  ###################################################################################
  ###################################################################################
  def assert_tlm_size(self, size, tlm_name=None, filterFunc=None):
    '''
    Assert the number of telemetry points received is equal to 'size'.
    @param size: the number of telemetry points expected
    @param tlm_name: (optional) if provided, only check the size of telemetry
    points of this type
    @param filterFunc: (optional) if provided, only telemetry values that return true when passed
    into this function are counted. For example, to only count telemetry values with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._tlm_hist.assert_size(size, tlm_name, filterFunc)

  def assert_tlm_size_eq(self, size, tlm_name=None, filterFunc=None): # Same as above, but here for convenience
    '''
    Assert the number of telemetry points received is equal to 'size'.
    @param size: the number of telemetry points expected
    @param tlm_name: (optional) if provided, only check the size of telemetry
    points of this type
    @param filterFunc: (optional) if provided, only telemetry values that return true when passed
    into this function are counted. For example, to only count telemetry values with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._tlm_hist.assert_size_eq(size, tlm_name, filterFunc)
    
  def assert_tlm_size_ne(self, size, tlm_name=None, filterFunc=None):
    '''
    Assert the number of telemetry points received is not equal to 'size'.
    @param size: the number of telemetry points expected
    @param tlm_name: (optional) if provided, only check the size of telemetry
    points of this type
    @param filterFunc: (optional) if provided, only telemetry values that return true when passed
    into this function are counted. For example, to only count telemetry values with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._tlm_hist.assert_size_ne(size, tlm_name, filterFunc)

  def assert_tlm_size_lt(self, size, tlm_name=None, filterFunc=None):
    '''
    Assert the number of telemetry points received is less than 'size'.
    @param size: the number of telemetry points expected
    @param tlm_name: (optional) if provided, only check the size of telemetry
    points of this type
    @param filterFunc: (optional) if provided, only telemetry values that return true when passed
    into this function are counted. For example, to only count telemetry values with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._tlm_hist.assert_size_lt(size, tlm_name, filterFunc)
    
  def assert_tlm_size_le(self, size, tlm_name=None, filterFunc=None):
    '''
    Assert the number of telemetry points received is less than or equal to 'size'.
    @param size: the number of telemetry points expected
    @param tlm_name: (optional) if provided, only check the size of telemetry
    points of this type
    @param filterFunc: (optional) if provided, only telemetry values that return true when passed
    into this function are counted. For example, to only count telemetry values with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._tlm_hist.assert_size_le(size, tlm_name, filterFunc)

  def assert_tlm_size_gt(self, size, tlm_name=None, filterFunc=None):
    '''
    Assert the number of telemetry points received is greater than 'size'.
    @param size: the number of telemetry points expected
    @param tlm_name: (optional) if provided, only check the size of telemetry
    points of this type
    @param filterFunc: (optional) if provided, only telemetry values that return true when passed
    into this function are counted. For example, to only count telemetry values with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._tlm_hist.assert_size_gt(size, tlm_name, filterFunc)

  def assert_tlm_size_ge(self, size, tlm_name=None, filterFunc=None):
    '''
    Assert the number of telemetry points received is greater than or equal to 'size'.
    @param size: the number of telemetry points expected
    @param tlm_name: (optional) if provided, only check the size of telemetry
    points of this type
    @param filterFunc: (optional) if provided, only telemetry values that return true when passed
    into this function are counted. For example, to only count telemetry values with numerical values 
    greater than 5 you can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self._tlm_hist.assert_size_ge(size, tlm_name, filterFunc)

  ###################################################################################
  ###################################################################################
  ## Test event values:
  ###################################################################################
  ###################################################################################
  def assert_evr(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is equal to 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_eq(value, evr_name, index)

  def assert_evr_eq(self, value, evr_name=None, index=history.ALL): # Same as above, but here for convenience
    '''
    Assert the value of event arguments received is equal to 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_eq(value, evr_name, index)

  def assert_evr_ne(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is not equal to 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_ne(value, evr_name, index)
    
  def assert_evr_lt(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is less than 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_lt(value, evr_name, index)
    
  def assert_evr_le(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is less than or equal to 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_le(value, evr_name, index)

  def assert_evr_gt(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is greater than 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_gt(value, evr_name, index)

  def assert_evr_ge(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is greater than or equal to 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_ge(value, evr_name, index)

  def assert_evr_is(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is (using identity function) 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_is(value, evr_name, index)

  def assert_evr_is_not(self, value, evr_name=None, index=history.ALL):
    '''
    Assert the value of event arguments received is not (using identity function) 'value'.
    @param value: the expected value of the event arguments
    @param evr_name: (optional) if provided, only check the value of events 
    of this type
    @param index: (optional) if provided, only check the value of events of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if evr_name is also specified.
    '''
    return self._evr_hist.assert_is_not(value, evr_name, index)

  ###################################################################################
  ###################################################################################
  ## Test telemetry values:
  ###################################################################################
  ###################################################################################
  def assert_tlm(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is equal to 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_eq(value, tlm_name, index)

  def assert_tlm_eq(self, value, tlm_name=None, index=history.ALL): # Same as above, but here for convenience
    '''
    Assert the value of telemetry received is equal to 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_eq(value, tlm_name, index)

  def assert_tlm_ne(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is not equal to 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_ne(value, tlm_name, index)

  def assert_tlm_lt(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is less than 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_lt(value, tlm_name, index)

  def assert_tlm_le(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is less than or equal to 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_le(value, tlm_name, index)

  def assert_tlm_gt(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is greater than 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_gt(value, tlm_name, index)

  def assert_tlm_ge(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is greater than or equal to 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_ge(value, tlm_name, index)

  def assert_tlm_is(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is (using identity function) 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_is(value, tlm_name, index)
    
  def assert_tlm_is_not(self, value, tlm_name=None, index=history.ALL):
    '''
    Assert the value of telemetry received is not (using identity function) 'value'.
    @param value: the expected value of the telemetry
    @param tlm_name: (optional) if provided, only check the value of tlm
    of this type
    @param index: (optional) if provided, only check the value of tlm of
    this index in the history. Passing TestHistory.ALL will check all indexes for that value.
    Passing TestHistory.ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if tlm_name is also specified.
    '''
    return self._tlm_hist.assert_is_not(value, tlm_name, index)

  ###################################################################################
  ###################################################################################
  ## Retrieve dictionary elements
  ###################################################################################
  ###################################################################################
  ## Note: Hopefully this API is good enough that you never need to use these functions.
  def get_evr(self, evr_name=None, index=history.ALL):
    '''
    Return the event history
    @param evr_name: (optional) only return the history for the given event type
    @param index: (optional) return the history for the given event type at the given index. Note
    that evr_name should also be specified with this argument, otherwise it is ignored
    @return the event history
    '''
    return self._evr_hist.get(evr_name, index)

  def get_tlm(self, tlm_name=None, index=history.ALL):
    '''
    Return the telemetry history
    @param tlm_name: (optional) only return the history for the given telemetry type
    @param index: (optional) return the history for the given event type at the given index. Note
    that evr_name should also be specified with this argument, otherwise it is ignored
    @return the telemetry history
    '''
    return self._tlm_hist.get(tlm_name, index)

