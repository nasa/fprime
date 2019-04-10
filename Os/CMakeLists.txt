####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Linux/File.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Linux/FileSystem.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Linux/InterruptLock.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Linux/IntervalTimer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Linux/WatchdogTimer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogPrintf.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MemCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Posix/Mutex.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Posix/Task.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Pthreads/BufferQueueCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Pthreads/MaxHeap/MaxHeap.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Pthreads/PriorityBufferQueue.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Pthreads/Queue.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/QueueCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/QueueString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SimpleQueueRegistry.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TaskCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TaskString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ValidateFileCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/X86/IntervalTimer.cpp"
)
set(MOD_DEPS
  "${CMAKE_THREAD_LIBS_INIT}" 
  Fw/Cfg
  Fw/Types
  Utils/Hash
)

register_fprime_module()

# Add stubs directory
##TODO-MSTARCH: add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Stubs/")

# Add unit test directories

##TODO-MSTARCH: add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
##TODO-MSTARCH: add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Pthreads/test/ut")
##TODO-MSTARCH: add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Pthreads/MaxHeap/test/ut")
