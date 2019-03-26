# Add source files for module

set(AUTOCODER_INPUT_FILES
  ""
)

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

set(LINK_DEPS
  "${CMAKE_THREAD_LIBS_INIT}" 
  Fw/Cfg
  Fw/Types
  Utils/Hash
)

# Invoke build system generate_module
generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")

# Add stubs directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Stubs/")

# Add unit test directories

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Pthreads/test/ut")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Pthreads/MaxHeap/test/ut")
