#
#   Copyright 2004-2008, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged. Any commercial use must be negotiated with the Office
#   of Technology Transfer at the California Institute of Technology.
#
#   Information included herein is controlled under the International
#   Traffic in Arms Regulations ("ITAR") by the U.S. Department of State.
#   Export or transfer of this information to a Foreign Person or foreign
#   entity requires an export license issued by the U.S. State Department
#   or an ITAR exemption prior to the export or transfer.
#

SRC = 			TaskCommon.cpp \
				TaskString.cpp \
				QueueCommon.cpp \
				QueueString.cpp \
				SimpleQueueRegistry.cpp \
				MemCommon.cpp \
				ValidateFileCommon.cpp 

HDR = 			Queue.hpp \
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
				FileSystem.hpp

SRC_LINUX=      Pthreads/Queue.cpp \
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

SRC_DARWIN =    Pthreads/Queue.cpp \
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
				Linux/FileSystem.cpp

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

