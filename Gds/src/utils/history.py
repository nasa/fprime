#!/bin/env python
#===============================================================================
# NAME: history.py
#
# DESCRIPTION: A test history for storage and assertion of items in the history
# AUTHOR: Kevin Dinkel
# EMAIL:  dinkel@jpl.nasa.gov
# DATE CREATED: November 19, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
import operator
import inspect
from controllers import event_loader
from controllers import channel_loader

__author__ = "Kevin Dinkel"
__copyright__ = "Copyright 2015, California Institute of Technology."
__version__ = "1.0"
__email__ = "kevin.dinkel@jpl.nasa.gov"

###############################
# Global variables:
###############################
'''
This is an object which specifies that all elements should be checked during a comparison.
It can be used as an input to index as follows:

# Ensure that all elements of type "foo" equal 1.
History.assert_eq(1, "foo", index=History.ALL)
'''
ALL = "ALL"

'''
This is an object which specifies that any of elements in the dictionary can satisfy a 
comparison. It can be used as an input to index as follows:

# If any element of "foo" equals 1, then this assertion will pass.
History.assert_eq(1, "foo", index=History.ANY)
'''
ANY= "ANY"

'''
This is an object which always returns True during a comparison. This can be used when
running assertions. For example, say a history element stores its value as [foo, boo, you].
but you only cate to assert against the first two elements, and do not care what the third
element is. You can achieve this by calling the method:

History.assert_eq([foo, boo, History.anything])

'''
class __wildcard(object):
  '''
  An object that always returns True when compared.
  '''
  def __eq__(self, other):
    return True

ANYTHING = __wildcard()

'''
This is an object that helps in comparing floating point numbers. Creating an object of
type 'near' will compare floating point values to within some epsilon. Here is an example.

History.assert_eq(History.near(3.1415, epsilon=0.01))
'''
class __close_to(object):
  '''
  An object that always returns True when compared.
  '''
  def __init__(self, value, epsilon = 10e-4):
    self.val = value
    self.eps = epsilon

  def __eq__(self, other):
    if other > self.val - self.eps and other < self.val + self.eps:
      return True
    return False

NEAR = __close_to

class History(object):
  '''
  This is a class which houses a python dictionary that stores lists of values
  indexed by names. This dictionary can be added to and cleared. This class
  provides a set of assertion functions to check the size (existance) and value
  of elements in the dictionary. The intention is that this class will be used 
  as a history for use with events and telemetry, and the assertions can be run
  as test cases.
  '''

  ###############################
  # Public API methods:
  ###############################
  def __init__(self, data=None):
    '''
    Constructor
    '''
    # Private history:
    self._hist = {}
    if data:
      self.add(data)
    super(History, self).__init__()

  def add(self, data):
    ''' 
    Add data to the dictionary
    @param data: a list of tuples in the form (name, value). The name valuies will be
    stored in the history, indexed by name
    '''
    for name, value in data:
      try:
        self._hist[name].append(value)
      except:
        self._hist[name] = [value]

  ###################################################################################
  ###################################################################################
  ## Clear histories:
  ###################################################################################
  ###################################################################################
  def clear(self):
    ''' 
    Remove all elements from history 
    '''
    self._hist.clear()

  ###################################################################################
  ###################################################################################
  ## Print helpers:
  ###################################################################################
  ###################################################################################
  def get_hist_str(self):
    ''' 
    Returns the contents of the history dictionary
    '''
    print_str = ""
    for name, values in self._hist.iteritems():
      print_str += "\nname: %s, count: %02d, args: %s" % (name, len(values), str(values))
   
    return print_str

  def print_hist(self, prnt=True):
    ''' 
    Print the contents of the history dictionary
    '''
    print self.get_hist_str()
   
  ###################################################################################
  ###################################################################################
  ## Test history size:
  ###################################################################################
  ###################################################################################
  def assert_size(self, size, name=None, filterFunc=None):
    '''
    Assert the number of history elements received is equal to 'size'.
    @param size: the number of history elements expected
    @param name: (optional) if provided, only check the size of history elements of
    this type
    @param filterFunc: (optional) if provided, only elements that return true when passed
    into this function are counted. For example, to only count values greater than 5 you
    can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self.__assert_hist_size(size, operator.eq, name, filterFunc)

  def assert_size_eq(self, size, name=None, filterFunc=None): # Same as above, but here for convenience
    '''
    Assert the number of history elements received is equal to 'size'.
    @param size: the number of history elements expected
    @param name: (optional) if provided, only check the size of history elements of
    this type
    @param filterFunc: (optional) if provided, only elements that return true when passed
    into this function are counted. For example, to only count values greater than 5 you
    can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self.__assert_hist_size(size, operator.eq, name, filterFunc)

  def assert_size_ne(self, size, name=None, filterFunc=None):
    '''
    Assert the number of history elements received is not equal to 'size'.
    @param size: the number of history elements expected
    @param name: (optional) if provided, only check the size of history elements of
    this type
    @param filterFunc: (optional) if provided, only elements that return true when passed
    into this function are counted. For example, to only count values greater than 5 you
    can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self.__assert_hist_size(size, operator.ne, name, filterFunc)

  def assert_size_lt(self, size, name=None, filterFunc=None):
    '''
    Assert the number of history elements received is less than 'size'.
    @param size: the number of history elements expected
    @param name: (optional) if provided, only check the size of history elements of
    this type
    @param filterFunc: (optional) if provided, only elements that return true when passed
    into this function are counted. For example, to only count values greater than 5 you
    can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self.__assert_hist_size(size, operator.lt, name, filterFunc)
    
  def assert_size_le(self, size, name=None, filterFunc=None):
    '''
    Assert the number of history elements received is less than or equal to 'size'.
    @param size: the number of history elements expected
    @param name: (optional) if provided, only check the size of history elements of
    this type
    @param filterFunc: (optional) if provided, only elements that return true when passed
    into this function are counted. For example, to only count values greater than 5 you
    can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self.__assert_hist_size(size, operator.le, name, filterFunc)

  def assert_size_gt(self, size, name=None, filterFunc=None):
    '''
    Assert the number of history elements received is greater than 'size'.
    @param size: the number of history elements expected
    @param name: (optional) if provided, only check the size of history elements of
    this type
    @param filterFunc: (optional) if provided, only elements that return true when passed
    into this function are counted. For example, to only count values greater than 5 you
    can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self.__assert_hist_size(size, operator.gt, name, filterFunc)

  def assert_size_ge(self, size, name=None, filterFunc=None):
    '''
    Assert the number of history elements received is greater than or equal to 'size'.
    @param size: the number of history elements expected
    @param name: (optional) if provided, only check the size of history elements of
    this type
    @param filterFunc: (optional) if provided, only elements that return true when passed
    into this function are counted. For example, to only count values greater than 5 you
    can pass in the function: filterFunc=(lambda x: x>5)
    '''
    return self.__assert_hist_size(size, operator.ge, name, filterFunc)

  ###################################################################################
  ###################################################################################
  ## Test history values:
  ###################################################################################
  ###################################################################################
  def assert_eq(self, value, name=None, index=ALL): # Same as above, but here for convenience
    '''
    Assert the value of history item received is equal to 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.eq, name, index)

  def assert_ne(self, value, name=None, index=ALL):
    '''
    Assert the value of history item received is not equal to 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.ne, name, index)
    
  def assert_lt(self, value, name=None, index=ALL):
    '''
    Assert the value of history item received is less than 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.lt, name, index)
    
  def assert_le(self, value, name=None, index=ALL):
    '''
    Assert the value of history item received is less than or equal to 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.le, name, index)

  def assert_gt(self, value, name=None, index=ALL):
    '''
    Assert the value of history item received is greater than 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.gt, name, index)

  def assert_ge(self, value, name=None, index=ALL):
    '''
    Assert the value of history item received is greater than or equal to 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.ge, name, index)

  def assert_is(self, value, name=None, index=ALL):
    '''
    Assert the value of history item received is (using identity function) 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.is_, name, index)

  def assert_is_not(self, value, name=None, index=ALL):
    '''
    Assert the value of history item received is not (using identity function) 'value'.
    @param value: the expected value of the history item
    @param name: (optional) if provided, only check the value of history 
    of this type
    @param index: (optional) if provided, only check the value of history of
    this index in the history. Passing ALL will check all indexes for that value.
    Passing ANY will check to make sure at least 1 value meets the comparison.
    Note index will only be used if name is also specified.
    '''
    return self.__assert_hist(value, operator.is_not, name, index)

  ###################################################################################
  ###################################################################################
  ## Retrieve dictionary elements
  ###################################################################################
  ###################################################################################
  def get(self, name=None, index=ALL):
    '''
    Return the history
    @param name: (optional) only return the history for the given history type
    @param index: (optional) return the history for the given history type at the given index. Note
    that name should also be specified with this argument, otherwise it is ignored
    @return the history
    '''
    if name:
      if name in self._hist:
        if index == ALL:
          return self._hist[name]
        else:
          return self._hist[name][index]
      else:
        return None
    else:
      return self._hist

  ###############################
  # Private methods:
  ###############################
 
  def __get_func_name(self, func):
    return func.__name__

  def __pretty_print(self, name, message):
    '''
    return the specified message in a printable format
    '''
    ret_str = "\n\n\n------- %s -------" % name
    ret_str += message
    ret_str += "\n"
    for i in xrange(16+len(name)):
      ret_str += "-"
    return ret_str

  def __single_id_history(self, id, name, lst):
    '''
    Returns the history for a single id in a printable format
    Name will default to id if not specified
    '''
    return self.__pretty_print("%s" % name, message = "\nname: %s, count: %02d, args: %s" % (id, len(lst), str(lst)))
    
  def __full_hist(self):
    '''
    Return the full history in a printable format
    '''
    return self.__pretty_print("Full History", self.get_hist_str())

  def __assert_hist_size(self, size, comp, id, filterFunc):
    ''' 
    Check the size of a dictionary, or dictionary element:
    If id is none, return true if the number of elements in the dictionary compares to size, else return false
    If id is specified, return true if the number of elements in id's entry in the dictionary compares to size, else return false

    The comparison to perform is passed by the function argument 'comp'
    '''
    def error_msg(func, size, comp_val, id=None):
      '''
      returns an error message on assert failure in a printable format
      '''
      if id:
        return "Asserting history size for name \"%s\" <%s> %s failed. Actual size: %s." % (id, self.__get_func_name(func), size, comp_val)
      else:
        return "Asserting total history size <%s> %s failed. Actual size: %s." % (self.__get_func_name(func), size, comp_val)

    def filtered_single_id_history(id, filterFunc=None, filterList=None):
      '''
      If a function and list are provided, returns the id's filtered history.
      Otherwise, returns an empty string
      '''
      if filterFunc:
        return self.__single_id_history(id, "%s History filtered by user function %s" % (id, self.__get_func_name(filterFunc)), filterList)
      else:
        return ""

    def get_filtered_hist(filterFunc=None):
      '''
      Return the full history with a filter applied in a printable format
      '''
      if filterFunc:
        hist = ""
        for key in self._hist:
          args = filter(filterFunc, self._hist[key])
          hist += "\nname: %s, count: %02d, args: %s" % (key, len(args), args)
        return self.__pretty_print("Full History filtered by user function %s" % filterFunc, hist)
      else:
        return ""

    if id:
      if id in self._hist:
        v = self._hist[id]
        if filterFunc:
          v = filter(filterFunc, v)
        assert comp(len(v), size), error_msg(comp, size, len(v), id=id) + \
                                   self.__single_id_history(id, id + " History", self._hist[id]) + \
                                   filtered_single_id_history(id, filterFunc=filterFunc, filterList=v) + \
                                   self.__full_hist()
      else:
        assert comp(0, size), error_msg(comp, size, 0, id=id) + self.__pretty_print(id + " History", "\nname: %s not found in history." % id) + self.__full_hist()
    else:
      v = self._hist
      if filterFunc:
        for i in v.itervalues():
          i = filter(filterFunc, i)
      total = sum(len(v) for v in self._hist.itervalues())
      assert comp(total, size), error_msg(comp, size, total) + get_filtered_hist(filterFunc) + self.__full_hist()

  def __assert_hist(self, value, comp, id, index):
    '''
    Check the value of a dictionary element:
    If id is none, check all values in the dictionary compared to value.
    If id exists, but index does not, check all the values that match that id in the dictionary compared to value.
    If id exists and index exists, check the dictionary element for that id and index compared to value.
    
    The comparison to perform is passed by the function argument 'comp'
    '''

    def compare(arg, value, string):
      assert comp(arg, value), string

    def idIndexChecker(value, comp, id, index):
      def f(arg):
        return compare(arg, value, "Asserting history value for name \"%s\" <%s> %s at index %s failed. Actual value: %s." % (id, self.__get_func_name(comp), value, index, arg) + \
                                  self.__single_id_history(id, id + " History", self._hist[id]) + \
                                  self.__full_hist())
      return f

    def idAllChecker(value, comp, id):
      def f(arg):
        return compare(arg, value, "Asserting history value for name \"%s\" <%s> %s on ALL indices failed. Found value of: %s." % (id, self.__get_func_name(comp), value, arg) + \
                                  self.__single_id_history(id, id + " History", self._hist[id]) + \
                                  self.__full_hist())
      return f
      
    def allChecker(value, comp):
      def f(arg):
        return compare(arg, value, "Asserting ALL history values <%s> %s failed. Found value of: %s." % (self.__get_func_name(comp), value, arg) + self.__full_hist())
      return f

    if id:
      if id in self._hist:
        if index == ALL:
          check = idAllChecker(value, comp, id)
          map(check, self._hist[id]) # check all
        elif index == ANY:
          check = idAllChecker(value, comp, id)
          for v in self._hist[id]:
            try:
              check(v)
              return # an index passed, so no need to check the rest
            except AssertionError:
              pass
          assert 0, "Asserting ANY history value for name \"%s\" <%s> %s failed." % (id, self.__get_func_name(comp), value) + \
                    self.__single_id_history(id, id + " History", self._hist[id]) + \
                    self.__full_hist()
        elif index < len(self._hist[id]):
          check = idIndexChecker(value, comp, id, index)
          check(self._hist[id][index])
        else:
          assert index < len(self._hist[id]), "Index %s does not exist in the history for name \"%s\". Actual \"%s\" history size: %s." % (index, id, id, len(self._hist[id])) + \
                                              self.__single_id_history(id, id + " History", self._hist[id]) + \
                                              self.__full_hist()
      else:
        assert 0, "Name \"%s\" not found in history." % id + self.__full_hist() 
    else:
      check = allChecker(value, comp)
      [map(check, self._hist[i]) for i in self._hist.keys()]

###############################
# Decorators:
###############################
def __check_name_decorator(func):
  ''' 
  Decorator for checking input name for functions in this api. For instance, this checker
  makes can be used to make sure that an evr with evr_name has been loaded and exists in the system
  before running any function that uses it.
  '''
  def checker(*args, **kwargs):
    # Extract self from arguments:
    name = None
    if "self" in kwargs:
      self = kwargs["self"]
    else:
      self = args[0]
    # Extract evr_name from arguments:
    if "name" in kwargs:
      name = kwargs["name"]
    else:
      nameIndex = func.__code__.co_varnames[:func.__code__.co_argcount].index("name")
      if nameIndex < len(args):
        name = args[nameIndex]
    # Check name to make sure it is valid:
    if name:
      if not self._name_valid(name):
        raise Exception(self._type_name.capitalize() + ": '" + name + "' does not exist in " + self._type_name+ " dictionary. Please use a valid " + self._type_name + " name.")
    return func(*args, **kwargs)
  return checker

def __checkName(decorator):
  def wrapper(cls):
    for name, func in inspect.getmembers(cls, predicate=inspect.ismethod):
      # make sure method is callable, public, and contains an argument named 'name' before we decorate it
      argnames = func.__code__.co_varnames[:func.__code__.co_argcount]
      if callable(func) and not name.startswith("_") and "name" in argnames:
        setattr(cls, name, decorator(func))
    return cls
  return wrapper

########################################
# Decorated specialized history classes:
########################################
@__checkName(__check_name_decorator)
class EventHistory(History):
  '''
  A history class with all public method arguments that include 'name' checked to make sure
  it is a valid evr name before running the method.
  '''
  _events = event_loader.EventLoader.getInstance()
  _type_name = "event"

  def _name_valid(self, name):
    return name in self._events.getEventsDictByName()

@__checkName(__check_name_decorator)
class TelemetryHistory(History):
  '''
  A history class with all public method arguments that include 'name' checked to make sure
  it is a valid telemetry name before running the method.
  '''
  _channels = channel_loader.ChannelLoader.getInstance()
  _type_name = "telemetry"

  def _name_valid(self, name):
    return name in self._channels.getChDictByName()
