################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L4_IO/fat/disk/spi_flash.cpp 

C_SRCS += \
../L4_IO/fat/disk/diskio.c \
../L4_IO/fat/disk/sd.c 

OBJS += \
./L4_IO/fat/disk/diskio.o \
./L4_IO/fat/disk/sd.o \
./L4_IO/fat/disk/spi_flash.o 

C_DEPS += \
./L4_IO/fat/disk/diskio.d \
./L4_IO/fat/disk/sd.d 

CPP_DEPS += \
./L4_IO/fat/disk/spi_flash.d 


# Each subdirectory must supply rules for building sources it contributes
L4_IO/fat/disk/%.o: ../L4_IO/fat/disk/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L0_LowLevel" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS/include" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS\portable" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L2_Drivers" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L3_Utils" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO\fat" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L5_Application" -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

L4_IO/fat/disk/%.o: ../L4_IO/fat/disk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C++ Compiler'
	arm-none-eabi-g++ -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L0_LowLevel" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS\include" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS\portable" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L2_Drivers" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L3_Utils" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO\fat" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L5_Application" -Os -Wall -Wa,-adhlns="$@.lst" -fno-exceptions -fno-rtti -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

