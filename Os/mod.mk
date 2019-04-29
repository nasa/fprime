#
#   Copyright 2004-2008, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

SRC = 			TaskCommon.cpp \
				TaskString.cpp \
				QueueCommon.cpp \
				QueueString.cpp \
				IPCQueueCommon.cpp \
				SimpleQueueRegistry.cpp \
				MemCommon.cpp \
				ValidateFileCommon.cpp \
				ValidatedFile.cpp \
				FileCommon.cpp

HDR = 			Queue.hpp \
				IPCQueue.hpp \
				QueueString.hpp \
				SimpleQueueRegistry.hpp \
				Task.hpp \
				TaskString.hpp \
				InterruptLock.hpp \
				IntervalTimer.hpp \
				WatchdogTimer.hpp \
				Mutex.hpp \
				File.hpp \
				ValidateFile.hpp \
				FileSystem.hpp \
				LocklessQueue.hpp \
				ValidatedFile.hpp

SRC_LINUX=      Posix/IPCQueue.cpp \
               	Pthreads/Queue.cpp \
               	Pthreads/BufferQueueCommon.cpp \
                Pthreads/PriorityBufferQueue.cpp \
                Pthreads/MaxHeap/MaxHeap.cpp \
				Linux/File.cpp \
				Posix/Task.cpp \
				LogPrintf.cpp \
				Linux/InterruptLock.cpp \
				Linux/WatchdogTimer.cpp \
				X86/IntervalTimer.cpp \
				Linux/IntervalTimer.cpp \
				Posix/Mutex.cpp \
				Linux/FileSystem.cpp \
				Posix/LocklessQueue.cpp

SRC_DARWIN =    MacOs/IPCQueueStub.cpp \ # NOTE(mereweth) - provide a stub that only works in single-process, not IPC
               	Pthreads/Queue.cpp \
                Pthreads/BufferQueueCommon.cpp \
                Pthreads/PriorityBufferQueue.cpp \
                Pthreads/MaxHeap/MaxHeap.cpp \
				Linux/File.cpp \
				Posix/Task.cpp \
				LogPrintf.cpp \
				Linux/WatchdogTimer.cpp \
				Linux/InterruptLock.cpp \
				X86/IntervalTimer.cpp \
				MacOs/IntervalTimer.cpp \
				Posix/Mutex.cpp \
				Linux/FileSystem.cpp  \
				Posix/LocklessQueue.cpp

SRC_CYGWIN =    Pthreads/Queue.cpp \
               	Pthreads/BufferQueueCommon.cpp \
                Pthreads/PriorityBufferQueue.cpp \
                Pthreads/MaxHeap/MaxHeap.cpp \
				Linux/File.cpp \
				Posix/Task.cpp \
				LogPrintf.cpp \
				Linux/InterruptLock.cpp \
				Linux/WatchdogTimer.cpp \
				X86/IntervalTimer.cpp \
				Linux/IntervalTimer.cpp \
				Posix/Mutex.cpp \
				Linux/FileSystem.cpp
				
SRC_RASPIAN =   Pthreads/Queue.cpp \
               	Pthreads/BufferQueueCommon.cpp \
                Pthreads/PriorityBufferQueue.cpp \
                Pthreads/MaxHeap/MaxHeap.cpp \
				Linux/File.cpp \
				Posix/Task.cpp \
				LogPrintf.cpp \
				Linux/InterruptLock.cpp \
				Linux/WatchdogTimer.cpp \
				X86/IntervalTimer.cpp \
				Linux/IntervalTimer.cpp \
				Posix/Mutex.cpp \
				Linux/FileSystem.cpp

				


SUBDIRS = test

# to use Pthread priority queue include:
#        Pthreads/Queue.cpp \
#        Pthreads/BufferQueueCommon.cpp \
#        Pthreads/PriorityBufferQueue.cpp \
#        Pthreads/MaxHeap/MaxHeap.cpp \


# to use Pthread fifo queue include:
#        Pthreads/Queue.cpp \
#        Pthreads/BufferQueueCommon.cpp \
#        Pthreads/FIFOBufferQueue.cpp \

