cmake_minimum_required(VERSION 3.15)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR Arm)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

# Project name
project(MIMXRT1052_Project_Demo LANGUAGES C ASM VERSION 1.0)

configure_file(TutorialConfig.h.in TutorialConfig.h)

enable_language(C ASM)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
#set(CMAKE_LINKER ${TOOLCHAIN_PREFIX})
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#set(TARGET_FLAGS "-mcpu=cortex-m7")
#set(CMAKE_C_LINK_ECECUTABLE "arm-none-eabi-ld")

# Specify the target processor
set(CPU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")
set(SDK_FLAGS "-DCPU_MIMXRT1052CVL5B \
		-DPRINTF_ADVANCED_ENABLE=1 \
		-DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
		-D__PERF_COUNTER__ \
		-DXIP_BOOT_HEADER_DCD_ENABLE \
		-DPRINTF_ADVANCED_ENABLE=1 \
		-D__SEMIHOST_HARDFAULT_DISABLE \
		-DCPU_MIMXRT1052CVL5B_cm7 \
		-DSDK_OS_BAREMETAL \
		-DXIP_EXTERNAL_FLASH=1 \
		-DXIP_BOOT_HEADER_ENABLE=1 \
		-DSERIAL_PORT_TYPE_UART=1 \
		-DSDK_DEBUGCONSOLE=1 \
		-DSDK_DEBUGCONSOLE_UART \
		-D__MCUXPRESSO \
		-DDEBUG_CONSOLE_RX_ENABLE=1 \
		-DDEBUG_CONSOLE_ENABLE_ECHO \
		-DDEBUG_CONSOLE_TX_RELIABLE_ENABLE \
		-DCR_INTEGER_PRINTF \
		-DSCANF_FLOAT_ENABLE=1 \
		-DPRINTF_FLOAT_ENABLE=1 \
		-D__USE_CMSIS -DDEBUG \
		-DDEBUG \
		-D__NEWLIB__ \
		-DTIMER_PORT_TYPE_GPT=1")

#-lnosys
set(C_FLAGS "-std=gnu11  -Wall -Wextra -O0 -fno-common -fmerge-constants -g3 -gdwarf-4  -fstack-usage -Wstack-usage=2048 -ffunction-sections -fdata-sections -ffreestanding -fno-builtin")

set(ASM_FLAGS "-x assembler-with-cpp -MMD -MP -Wall -O0 -fno-common -fmerge-constants -g3 -fstack-usage -fdata-sections -ffunction-sections")

#set(CXX_FLAG "")

# Compiler options
set(CMAKE_C_FLAGS "${CPU_FLAGS} ${SDK_FLAGS} ${C_FLAGS}")
set(CMAKE_ASM_FLAGS "${ASM_FLAGS} ${CPU_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CXX_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# Linker script
#set(CMAKE_EXE_LINKER_FLAGS "-T /home/jiyongjie/mimxrt1052_project_demo/MIMXRT1052_Project_Demo_Debug.ld ${CPU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-T MIMXRT1052_Project_Demo_Debug.ld -L../Debug ${CPU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nosys.specs --specs=nano.specs")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")

file(GLOB_RECURSE SOURCES "board/*.c" 
    "component/crc/*.c"
    "component/gpio/*.c"
    "component/led/*.c"
    "component/lists/*.c"
    "component/reset/*.c"
    "component/serial_manager/*.c"
    "component/timer/*.c"
    "component/timer_manager/*.c"
    "component/uart/*.c"
    "device/*.c"
    "drivers/*.c"
    "source/bsp/*.c"
    "source/cm_backtrace/*.c"
    "source/coremark/*.c"
    "source/easylogger/port/*.c"
    "source/easylogger/src/*.c"
    "source/letter-shell/src/*.c"
    "source/lwrb/*.c"
    "source/openblt/Target/Source/ARMCM7_NXP_IMX1052/*.c"
    "source/openblt/Target/Source/*.c"
    "source/perf_counter/*.c"
    "source/MIMXRT1052_Project_Demo.c"
    "source/syscall.c"
    "startup/startup_mimxrt1052.c"
    "utilities/*.c"
    "utilities/utilities/*.c"
    "xip/*.c"
    "source/cm_backtrace/fault_handler/gcc/cmb_fault.S"
    "utilities/jump_app.S"
    "utilities/fsl_memcpy.S"
    )
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "board/"
    "CMSIS/"
    "component/crc/"
    "component/gpio/"
    "component/led/"
    "component/lists/"
    "component/reset/"
    "component/serial_manager/"
    "component/timer/"
    "component/timer_manager/"
    "component/uart/"
    "device/"
    "drivers/"
    "source/"
    "source/bsp/"
    "source/cm_backtrace/"
    "source/cm_backtrace/fault_handler/"
    "source/cm_backtrace/Languages/en-US/"
    "source/cm_backtrace/Languages/zh-CN/"
    "source/coremark/"
    "source/easylogger/inc/"
    "source/letter-shell"
    "source/letter-shell/src/"
    "source/lwrb/"
    "source/openblt/Target/Source/"
    "source/openblt/Target/Source/ARMCM7_NXP_IMX1052/"
    "source/perf_counter"
    "utilities/"
    "utilities/utilities"
    "xip/"
    )

#target_link_directories(${PROJECT_NAME} PRIVATE )

# Post-build commands to generate binary and hex files
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Generating binary and hex files"
)

