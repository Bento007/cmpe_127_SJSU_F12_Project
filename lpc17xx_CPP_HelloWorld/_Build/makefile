################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include L5_Application/source/subdir.mk
-include L5_Application/subdir.mk
-include L4_IO/src/subdir.mk
-include L4_IO/fat/option/subdir.mk
-include L4_IO/fat/disk/subdir.mk
-include L4_IO/fat/subdir.mk
-include L3_Utils/src/subdir.mk
-include L2_Drivers/src/subdir.mk
-include L1_FreeRTOS/src/subdir.mk
-include L1_FreeRTOS/portable/subdir.mk
-include L1_FreeRTOS/hooks/subdir.mk
-include L1_FreeRTOS/MemMang/subdir.mk
-include L0_LowLevel/source/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
lpc17xx_CPP_HelloWorld.hex \

SECONDARY_LIST += \
lpc17xx_CPP_HelloWorld.lst \

SECONDARY_SIZE += \
lpc17xx_CPP_HelloWorld.siz \


# All Target
all: lpc17xx_CPP_HelloWorld.elf secondary-outputs

# Tool invocations
lpc17xx_CPP_HelloWorld.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Yagarto Windows GCC C++ Linker'
	arm-none-eabi-g++ -T"..\loader.ld" -nostartfiles -Wl,-Map,lpc17xx_CPP_HelloWorld.map -mcpu=cortex-m3 -mthumb -o "lpc17xx_CPP_HelloWorld.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lpc17xx_CPP_HelloWorld.hex: lpc17xx_CPP_HelloWorld.elf
	@echo 'Invoking: ARM Yagarto Windows GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex lpc17xx_CPP_HelloWorld.elf  "lpc17xx_CPP_HelloWorld.hex"
	@echo 'Finished building: $@'
	@echo ' '

lpc17xx_CPP_HelloWorld.lst: lpc17xx_CPP_HelloWorld.elf
	@echo 'Invoking: ARM Yagarto Windows GNU Create Listing'
	arm-none-eabi-objdump -h -S lpc17xx_CPP_HelloWorld.elf > "lpc17xx_CPP_HelloWorld.lst"
	@echo 'Finished building: $@'
	@echo ' '

lpc17xx_CPP_HelloWorld.siz: lpc17xx_CPP_HelloWorld.elf
	@echo 'Invoking: ARM Yagarto Windows GNU Print Size'
	arm-none-eabi-size  --format=berkeley lpc17xx_CPP_HelloWorld.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(SECONDARY_FLASH)$(CXX_DEPS)$(S_UPPER_DEPS)$(SECONDARY_LIST)$(C++_DEPS)$(SECONDARY_SIZE)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS) lpc17xx_CPP_HelloWorld.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
