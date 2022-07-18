################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32H7xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
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

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := HPR_Nuclino_Using_CMSIS_MATH_LIBRARY
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.list \

OBJCOPY_BIN += \
HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.bin \


# All Target
all: main-build

# Main-build Target
main-build: HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.elf secondary-outputs

# Tool invocations
HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.elf: $(OBJS) $(USER_OBJS) C:\Users\chama\STM32CubeIDE\ARM_MATH_CM7_testing\HPR_Nuclino_Using_CMSIS_MATH_LIBRARY\STM32H7A3ZITXQ_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"C:\Users\chama\STM32CubeIDE\ARM_MATH_CM7_testing\HPR_Nuclino_Using_CMSIS_MATH_LIBRARY\STM32H7A3ZITXQ_FLASH.ld" --specs=nosys.specs -Wl,-Map="HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.map" -Wl,--gc-sections -static -L"C:\Users\chama\STM32CubeIDE\ARM_MATH_CM7_testing\HPR_Nuclino_Using_CMSIS_MATH_LIBRARY\libs" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.list"
	@echo 'Finished building: $@'
	@echo ' '

HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.bin HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.elf HPR_Nuclino_Using_CMSIS_MATH_LIBRARY.list default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
