################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/MIMXRT1052_Project.c \
../source/semihost_hardfault.c 

C_DEPS += \
./source/MIMXRT1052_Project.d \
./source/semihost_hardfault.d 

OBJS += \
./source/MIMXRT1052_Project.o \
./source/semihost_hardfault.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MIMXRT1052CVL5B -DCPU_MIMXRT1052CVL5B_cm7 -DSDK_OS_BAREMETAL -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\board" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\source" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\utilities" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\component\lists" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\component\serial_manager" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\component\uart" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\xip" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\drivers" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\CMSIS" -I"C:\Users\yongjie\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MIMXRT1052_Project\device" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/MIMXRT1052_Project.d ./source/MIMXRT1052_Project.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o

.PHONY: clean-source

