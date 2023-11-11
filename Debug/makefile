################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include MCAL/I2C/Source/subdir.mk
-include MCAL/DIO/Source/subdir.mk
-include MCAL/ADC/Source/subdir.mk
-include HAL/SevSeg/Source/subdir.mk
-include HAL/LCD/Source/subdir.mk
-include HAL/Keypad/Source/subdir.mk
-include App/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
SnakeGame.lss \

FLASH_IMAGE += \
SnakeGame.hex \

EEPROM_IMAGE += \
SnakeGame.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: SnakeGame.elf secondary-outputs

# Tool invocations
SnakeGame.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,SnakeGame.map -mmcu=atmega32 -o "SnakeGame.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SnakeGame.lss: SnakeGame.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S SnakeGame.elf  >"SnakeGame.lss"
	@echo 'Finished building: $@'
	@echo ' '

SnakeGame.hex: SnakeGame.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex SnakeGame.elf  "SnakeGame.hex"
	@echo 'Finished building: $@'
	@echo ' '

SnakeGame.eep: SnakeGame.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex SnakeGame.elf  "SnakeGame.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: SnakeGame.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 SnakeGame.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) SnakeGame.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
