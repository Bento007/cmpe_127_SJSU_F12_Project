################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L2_Drivers/src/I2C2.cpp \
../L2_Drivers/src/i2c_base.cpp \
../L2_Drivers/src/uart0.cpp \
../L2_Drivers/src/uart_base.cpp 

C_SRCS += \
../L2_Drivers/src/rtc.c \
../L2_Drivers/src/spi1.c 

OBJS += \
./L2_Drivers/src/I2C2.o \
./L2_Drivers/src/i2c_base.o \
./L2_Drivers/src/rtc.o \
./L2_Drivers/src/spi1.o \
./L2_Drivers/src/uart0.o \
./L2_Drivers/src/uart_base.o 

C_DEPS += \
./L2_Drivers/src/rtc.d \
./L2_Drivers/src/spi1.d 

CPP_DEPS += \
./L2_Drivers/src/I2C2.d \
./L2_Drivers/src/i2c_base.d \
./L2_Drivers/src/uart0.d \
./L2_Drivers/src/uart_base.d 


# Each subdirectory must supply rules for building sources it contributes
L2_Drivers/src/%.o: ../L2_Drivers/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C++ Compiler'
	arm-none-eabi-g++ -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L0_LowLevel" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS\include" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS\portable" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L2_Drivers" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L3_Utils" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO\fat" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L5_Application" -Os -Wall -Wa,-adhlns="$@.lst" -fno-exceptions -fno-rtti -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

L2_Drivers/src/%.o: ../L2_Drivers/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Yagarto Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L0_LowLevel" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS/include" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L1_FreeRTOS\portable" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L2_Drivers" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L3_Utils" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L4_IO\fat" -I"C:\Users\Trent Smith\Desktop\SJSU_Dev\projects\lpc17xx_CPP_HelloWorld\L5_Application" -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


