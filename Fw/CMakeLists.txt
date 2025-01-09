# Module subdirectories
set(FPRIME_FRAMEWORK_MODULES Fw_Prm Fw_Cmd Fw_Log Fw_Tlm Fw_Com Fw_Time Fw_Port Fw_Types Fw_Cfg CACHE INTERNAL "Fw mods")
# Port subdirectories
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Buffer/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Cmd/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Com/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Dp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Log/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Logger/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Prm/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Time/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Tlm/")

# Framework subdirectories
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Cfg/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Comp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Sm/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FilePacket/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Obj/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Port/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ports/CompletionStatus")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ports/Signal")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ports/SuccessCondition")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SerializableFile/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Test/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types/")

# Setup an interface target for Fw for efficiency
add_library(Fw INTERFACE)
add_dependencies(Fw ${FPRIME_FRAMEWORK_MODULES})
