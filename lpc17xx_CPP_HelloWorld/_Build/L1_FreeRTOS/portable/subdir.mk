################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../L1_FreeRTOS/portable/port.c 

OBJS += \
./L1_FreeRTOS/portable/port.o 

C_DEPS += \
./L1_FreeRTOS/portable/port.d 


# Each subdirectory must supply rules for building sources it contributes
L1_FreeRTOS/portable/%.o: ../L1_FreeRTOS/portable/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L0_LowLevel" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS/include" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS\portable" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L2_Drivers" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L3_Utils" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO\fat" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L5_Application" -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


