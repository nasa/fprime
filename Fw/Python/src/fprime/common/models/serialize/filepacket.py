#!/bin/env python
#===============================================================================
# NAME: filepacket.py
#
# DESCRIPTION: A python version of Rob Bocchino's filepacket classes
# AUTHOR: jdperez
# EMAIL:  jdperez@jpl.nasa.gov
# DATE CREATED: June 17, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
from __future__ import print_function
from __future__ import absolute_import

from abc import ABCMeta, abstractmethod

from .u32_type import *
from .u16_type import *
from .u8_type import *

from enum import Enum



#PacketType = Enum('PacketType', 'START DATA END CANCEL NONE')
class PacketType(Enum):
   START = 0
   DATA  = 1
   END   = 2
   CANCEL= 3
   NONE  = 255


class FilePacketAbc(object, metaclass=ABCMeta):
   @abstractmethod
   def serialize(self):
      pass
   @abstractmethod
   def deserialize(self):
      pass
   @abstractmethod
   def getBufSize(self):
      pass




class Header(FilePacketAbc):
   def __init__(self, pktType, seqIdx):
      self.__pktType = U8Type(pktType)
      self.__seqIdx = U32Type(seqIdx)

   def deserialize(self, buf):
      ptr = 0

      self.__pktType.deserialize(buf, ptr)
      ptr += self.__pktType.getSize()

      self.__seqIdx.deserialize(buf, ptr)

   def serialize(self):
      return self.__pktType.serialize() +\
         self.__seqIdx.serialize()

   def getBufSize(self):
      return self.__pktType.getSize() +\
         self.__seqIdx.getSize()


class StartPacket(FilePacketAbc):
   def __init__(self, seqIdx=None, filesize=None, srcPath="", destPath=""):
      self.__header = Header(PacketType.START.value, seqIdx)
      self.__filesize = U32Type(filesize)
      self.__srcPathLen = U8Type(len(srcPath))
      self.__srcPath = srcPath
      self.__destPathLen = U8Type(len(destPath))
      self.__destPath = destPath

   def setSrcPath(self, path):
      self.__srcPathLen = len(path)
      self.__srcPath = path
   def setDestPath(self, path):
      self.__destPathLen = len(path)
      self.__destPath = path
   def getDestPath(self):
      return self.__destPath
   def getSrcPath(self):
      return self.__srcPath
   def getSrcLen(self):
      return self.__srcPathLen
   def getDestLen(self):
      return self.__destPathLen
   def getFileSize(self):
      return self.__filesize

   def serialize(self):
      return self.__header.serialize() +\
         self.__filesize.serialize() +\
         self.__srcPathLen.serialize() +\
         self.__srcPath +\
         self.__destPathLen.serialize() +\
         self.__destPath

   def deserialize(self, buf):
      ptr = 0

      self.__header.deserialize(buf)
      ptr += self.__header.getBufSize()

      self.__filesize.deserialize(buf, ptr)
      ptr += self.__filesize.getSize()

      self.__srcPathLen.deserialize(buf, ptr)
      ptr += self.__srcPathLen.getSize()

      self.__srcPath = buf[ptr:self.__srcPathLen.val]
      ptr += self.__srcPathLen.val

      self.__destPathLen.deserialize(buf, ptr)
      ptr += self.__destPathLen.getSize()

      self.__destPath = buf[ptr:]

   def getBufSize(self):
      return self.__header.getBufSize() +\
         self.__filesize.getSize() +\
         self.__srcPathLen.getSize() +\
         self.__srcPathLen.val +\
         self.__destPathLen.getSize() +\
         self.__destPathLen.val

class DataPacket:
   def __init__(self, seqIdx=None, byteOffset=None, dataSize=None, data=None):
      self.__header = Header(PacketType.DATA.value, seqIdx)
      self.__byteOffset = U32Type(byteOffset)
      self.__dataSize = U16Type(dataSize)
      self.__data = data

   def serialize(self):
      return self.__header.serialize() +\
         self.__byteOffset.serialize() +\
         self.__dataSize.serialize() +\
         self.__data

   def deserialize(self, buf):
      ptr = 0

      self.__header.deserialize(buf)
      ptr += self.__header.getBufSize()

      self.__byteOffset.deserialize(buf, ptr)
      ptr += self.__byteOffset.getSize()

      self.__dataSize.deserialize(buf, ptr)
      ptr += self.__dataSize.getSize()

      self.__data = buf[ptr:]

   def getBufSize(self):
      return self.__header.getBufSize() +\
         self.__byteOffset.getSize() +\
         self.__dataSize.getSize() +\
         self.__dataSize.val

   def getHeader(self):
      return self.__header
   def getByteOffset(self):
      return self.__byteOffset
   def getDataSize(self):
      return self.__dataSize
   def getData(self):
      return self.__data

class EndPacket:
   def __init__(self, seqIdx=None, xSum=None):
      self.__header = Header(PacketType.END.value, seqIdx)
      self.__xSum = U32Type(xSum)

   def setChecksum(self, xSum):
      self.__xSum = xSum

   def getChecksum(self):
      return self.__xSum

   def serialize(self):
      return  self.__header.serialize() +\
         self.__xSum.serialize()

   def deserialize(self, buf):
      ptr = 0

      self.__header.deserialize(buf)
      ptr += self.__header.getBufSize()

      self.__xSum.deserialize(buf, ptr)

   def getBufSize(self):
      return self.__header.getBufSize() +\
         self.__xSum.getSize()


class CancelPacket:
   def __init__(self, seqIdx=None):
      self.__header = Header(PacketType.CANCEL.value, seqIdx)


class FilePacket:
   def __init__(self, filePacket):
      self.__filePacket = filePacket

   def serialize(self):
      #return self.__filePacket.header.pktType.serialize() +\
      #   self.__filePacket.header.seqIdx.serialize() +\
      #   self.__filePacket.serialize()
      return self.__filePacket.serialize()

   def deserialize(self, buf):
      ptr = 0

      """
      self.__filePacket.header.pktType.deserialize(buf, ptr)
      ptr += self.__filePacket.header.pktType.getSize()

      self.__filePacket.header.seqIdx.deserialize(buf, ptr)
      ptr += self.__seqIdx.getSize()

      self.__filePacket.deserialize(buf, ptr)
      """
      self.__filePacket.deserialize(buf, ptr)


   def getBufSize(self):
      return self.__filePacket.getBufSize()
