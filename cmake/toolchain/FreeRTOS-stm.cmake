set(CMAKE_SYSTEM_NAME "FreeRTOS-stm")
set(CMAKE_SYSTEM_PROCESSOR arm)

# enable the _WORKS flags to force local search on compiler
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Heaps of work here setting compiler flags for arm toolchain !!!!!!!!!!!!!!!
set(CMAKE_C_COMPILER "/usr/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/arm-none-eabi-g++")

SET(STM32_BSP_PATH "${CMAKE_CURRENT_LIST_DIR}/../../stm32-bsp" CACHE PATH "Path to STM32 BSP")
set(MCU_LINKER_SCRIPT ${STM32_BSP_PATH}/STM32H743ZITx_FLASH.ld)

#Setting linker options https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html#Link-Options:~:text=%2Dllibrary,%2Dl%20library
#-T Use script as the linker script.
#-l<library> Search the library named library when linking.
#
#-Wl,option Pass option as an option to the linker.
#--specs, https://launchpadlibrarian.net/287100883/readme.txt
#-m ARM options: https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
set(STM32_LD  "-T${MCU_LINKER_SCRIPT} \
    -mcpu=cortex-m7 \
    -mthumb \
    -mfpu=fpv5-sp-d16 \
    -mfloat-abi=hard \
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map \
    --specs=nosys.specs \
    -Wl,--start-group \
    -lc \
    -lm \
    -lstdc++ \
    -lsupc++ \
    -Wl,--end-group") 
set(COMPILER_COMMON_FLAGS
    "-fdata-sections -ffunction-sections \
    -DUSE_HAL_DRIVER \
    -DTGT_OS_TYPE_FREERTOS \
    -mcpu=cortex-m7 \
    -mthumb \
    -mfpu=fpv5-sp-d16 \
    -mfloat-abi=hard \
    -DSTM32H743xx \
    --specs=nano.specs -Wl,--gc-sections"
    )    
set(CMAKE_C_FLAGS
    "${COMPILER_COMMON_FLAGS} \
    -std=c99"
    )  
set(CMAKE_CXX_FLAGS
    "${COMPILER_COMMON_FLAGS} \
    -std=c++14 \
    -fno-rtti \
    -fno-exceptions \
    -fno-threadsafe-statics"
    )
set(CMAKE_EXE_LINKER_FLAGS ${STM32_LD})


# STEP 4: Specify paths to root of toolchain package, for searching for
#         libraries, executables, etc.
# set(CMAKE_FIND_ROOT_PATH  "/workspaces/fprime/gcc-arm-none-eabi-10.3-2021.10/")
set(CMAKE_FIND_ROOT_PATH  NEVER)

# DO NOT EDIT: F prime searches the host for programs, not the cross
# compile toolchain
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# DO NOT EDIT: F prime searches for libs, includes, and packages in the
# toolchain when cross-compiling.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
