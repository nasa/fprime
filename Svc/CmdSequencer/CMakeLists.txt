####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CmdSequencer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/CmdSequencerImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Events.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FPrimeSequence.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sequence.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/formats/AMPCSSequence.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/CmdSequencer/CmdSequencer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/AMPCS.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandBuffers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Health.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ImmediateBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Immediate.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ImmediateEOS.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Interceptors.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/InvalidFiles.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Mixed.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/MixedRelativeBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/NoFiles.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Relative.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/AMPCS/CRCs.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/AMPCS/Headers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/AMPCS/Records.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/BadCRCFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/BadDescriptorFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/BadTimeBaseFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/BadTimeContextFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/Buffers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/DataAfterRecordsFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/EmptyFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/File.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/FPrime/CRCs.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/FPrime/Headers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/FPrime/Records.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/ImmediateEOSFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/ImmediateFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/MissingCRCFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/MissingFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/MixedFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/RelativeFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/SizeFieldTooLargeFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/SizeFieldTooSmallFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/TooLargeFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SequenceFiles/USecFieldTooShortFile.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/JoinWait.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
)
set(UT_MOD_DEPS "Os_Stubs")
register_fprime_ut()
