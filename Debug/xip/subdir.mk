################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../xip/evkbimxrt1050_flexspi_nor_config.c \
../xip/fire_imxrt1052_sdram_ini_dcd.c \
../xip/fsl_flexspi_nor_boot.c 

C_DEPS += \
./xip/evkbimxrt1050_flexspi_nor_config.d \
./xip/fire_imxrt1052_sdram_ini_dcd.d \
./xip/fsl_flexspi_nor_boot.d 

OBJS += \
./xip/evkbimxrt1050_flexspi_nor_config.o \
./xip/fire_imxrt1052_sdram_ini_dcd.o \
./xip/fsl_flexspi_nor_boot.o 


# Each subdirectory must supply rules for building sources it contributes
xip/%.o: ../xip/%.c xip/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MIMXRT1052CVL5B -DCPU_MIMXRT1052CVL5B_cm7 -DSDK_OS_BAREMETAL -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\board" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\source" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\utilities" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\component\lists" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\component\serial_manager" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\component\uart" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\xip" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\drivers" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\CMSIS" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\device" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-xip

clean-xip:
	-$(RM) ./xip/evkbimxrt1050_flexspi_nor_config.d ./xip/evkbimxrt1050_flexspi_nor_config.o ./xip/fire_imxrt1052_sdram_ini_dcd.d ./xip/fire_imxrt1052_sdram_ini_dcd.o ./xip/fsl_flexspi_nor_boot.d ./xip/fsl_flexspi_nor_boot.o

.PHONY: clean-xip

