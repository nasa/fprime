import os
import glob
import numpy as np
import Tkinter
import tkFont

import ConfigManager


### 
# gse_misc.py
# 
# Miscellaneous utility methods for Gse
###




# Support custom entry validation
import Pmw

def hex_integer_validate(text):
  """
  Validate both hex and integer values
  """
  if Pmw.hexadecimalvalidator(text):
    return 1
  if Pmw.integervalidator(text):
    return 1
  return -1

def hex_integer_stringtovalue(text):
  """
  Convert a string value to its appropriate
  numerical value.
  """
  if text == '' or text=='-':
    return 0

  if '0x' in text:
    if text == '0x' or text=='-0x':
      return 0

    return int(text,16)
  else:
    return int(text)


# Start up utilities
def backwards_search(nlevels, dirname):
    """
    Walk backwards nlevels and search for 
    dirname. If found return path. Else
    return None.
    """
    stop = None

    # Search current directory
    if dirname in glob.glob("./"):
      stop = "./" 
    
    else:
     # Search through nlevels
      l = [(a+1)*'../' for a in range(nlevels)]
      for lvl in l:
        dir_list = glob.glob("{}/*".format(lvl)) 
        if dirname in ''.join(dir_list):
          # We found it! Now create the relative path
          stop = os.path.join(lvl, dirname)
          break

    if stop is not None:
      return os.path.abspath(stop)
    else:
      return None


# Utility methods to fix dropdown menus
import Tkinter

def __comboBox_postList(event, cb):
    """
    Event handler for combobox's Up and Down keys
    The key press inserts ascii into the entryWidget
    so we must delete it.

    Return break to stop event from propogating. 
    """
    #cb._postList()
    cb._entryWidget.delete(Tkinter.END)
    return 'break'



def rebind_comboBox(comboBox):
    """
    Rebind Up and Down key events to open menu instead of crashing
    """
    comboBox._entryWidget.unbind("<Up>")
    comboBox._entryWidget.unbind("<Down")
    comboBox._entryWidget.bind("<Up>", lambda e,cb=comboBox: __comboBox_postList(e,cb))
    comboBox._entryWidget.bind("<Down>", lambda e,cb=comboBox: __comboBox_postList(e,cb))



class HyperlinkManager(object):
  """
  Tkinter Text Widget Hyperlink manager from:
  http://effbot.org/zone/tkinter-text-hyperlink.htm
  """

  def __init__(self, text, justify=Tkinter.LEFT):

      self.text = text

      config = ConfigManager.ConfigManager.getInstance()
      font = tkFont.Font(family='Helvetica', size=int(config.get("helppanel", "default_header_link_size")))
      self.text.tag_config("hyper", font=font, foreground="blue", underline=1, justify=justify, tabs='15c')

      self.text.tag_bind("hyper", "<Enter>", self._enter)
      self.text.tag_bind("hyper", "<Leave>", self._leave)
      self.text.tag_bind("hyper", "<Button-1>", self._click)

      self.reset()

  def reset(self):
      self.links = {}

  def add(self, action, link_name):
      # add an action to the manager.  returns tags to use in
      # associated text widget
      tag = "hyper-%d" % len(self.links)
      self.links[tag] = (action, link_name)
      return "hyper", tag

  def _enter(self, event):
      self.text.config(cursor="hand2")

  def _leave(self, event):
      self.text.config(cursor="")

  def _click(self, event):
      for tag in self.text.tag_names(Tkinter.CURRENT):
          if tag[:6] == "hyper-":
              action    = self.links[tag][0]
              link_name = self.links[tag][1]
              action(link_name)
              return



class CircularBuffer(object):
  """
  Fixed length circular buffer. 
  """
  def __init__(self, max_len):
    self.__data    = np.zeros(max_len)
    self.__max_len = max_len
    self.__max_idx = max_len - 1
    self.__idx     = 0
    self.__tail    = 0
    self.__total   = 0

  def add(self, datapoint):

    self.__data[self.__idx] = datapoint

    self.__idx   += 1
    self.__total += 1

    # Reset index when it reaches end
    if self.__idx > self.__max_idx:
      self.__idx = 0

    # Buffer is full. Tail can move
    if self.__total > self.__max_len:
      self.__tail += 1
    
    # Reset tail when it reaches end
    if self.__tail > self.__max_idx:
      self.__tail = 0


  def asArray(self):

    # Array is not yet full 
    if self.__total < self.__max_idx:
      return self.__data[:self.__idx]

    # Array has cycled
    else:
      start = -(self.__max_len - self.__idx)
      r = range(start, self.__idx)
      return np.take(self.__data, r, mode="wrap")
      

  def getData(self):
    return self.__data

  def getHead(self):
    """
    Get most recent data point
    """
    return self.__data[self.__idx - 1]

  def getTail(self):
    """
    Get last data point
    """
    return self.__data[self.__tail]



def test_CircularBuffer():
  max_len = 100
  offset  = 5
  mostRecent = 104
  oldest = 5

  r = CircularBuffer(max_len)
  for i in range(max_len + offset):
    r.add(i)
  
  correct = np.array(range(offset, max_len + offset)).astype(float)

  assert np.array_equal(correct,r.asArray())
  assert mostRecent == r.getMostRecent()
  assert oldest    == r.getOldest()






