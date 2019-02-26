# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CmdSequencerComponentAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CmdSequencerImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Events.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FPrimeSequence.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sequence.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/formats/AMPCSSequence.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

# Add unit test directory
#TODO: CmdSequencer UTs don't work in CMake
#add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
