#!/bin/env python
#===============================================================================
# NAME: checksum.py
#
# DESCRIPTION: A python version of Rob Bocchino's checksum function from CFDP
# AUTHOR: jdperez
# EMAIL:  jdperez@jpl.nasa.gov
# DATE CREATED: June 17, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules

class Checksum:
   def __init__(self, xSum=0):
      self.xSum = xSum

   def __eq__(self, other):
      return self.xSum == other

   def __ne__(self, other):
      return not (self.xSum == other)

   def update(self, data, offset, length):
      index = 0

      offsetMod4 = offset % 4
      if offsetMod4 != 0:
         wordLength = min(length, 4 - offsetMod4)
         self.__addWordUnaligned(data[index:], offset + index, wordLength)
         index += wordLength

      while (index + 4 <= length):
         self.__addWordAligned(data[index:])
         index += 4

      if index < length:
         wordLength = length - index
         self.__addWordUnaligned(data[index:], offset + index, wordLength)
   @property
   def xSum(self):
      return self.xSum

   @xSum.setter
   def xSum(self, xSum):
      self.xSum = xSum

   def __addWordAligned(self, word):
      for i in range(4):
         self.__addByteAtOffset(word[i], i)

   def __addWordUnaligned(self, word, position, length):
      offset = position % 4
      for i in range(length):
         self.__addByteAtOffset(word[i], offset)
         offset += 1
         if offset == 4:
            offset = 0

   def __addByteAtOffset(self, byte, offset):
      addend = self.__mls(ord(byte), (8*(3-offset)))
      self.xSum = int(self.__madd(addend, self.xSum))

   def __madd(self, a, b):
      return (a + b) & 0xffffffffL 

   def __mls(self, a, b):
      return (a << b) & 0xffffffffL  
