################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/EFM32GG/subdir.mk
-include BSP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Table_Tennis_Detection.axf

# Tool invocations
Table_Tennis_Detection.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "Table_Tennis_Detection.ld" -Xlinker --gc-sections -Xlinker -Map="Table_Tennis_Detection.map" --specs=nano.specs -o Table_Tennis_Detection.axf "./BSP/bsp_bcc.o" "./BSP/bsp_stk.o" "./BSP/bsp_stk_leds.o" "./BSP/bsp_trace.o" "./CMSIS/EFM32GG/startup_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./emlib/em_adc.o" "./emlib/em_assert.o" "./emlib/em_cmu.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_leuart.o" "./emlib/em_prs.o" "./emlib/em_system.o" "./emlib/em_usart.o" "./src/adc.o" "./src/dmactrl.o" "./src/fix16.o" "./src/fix16_str.o" "./src/main.o" "./src/math_functions.o" "./src/spi.o" "./src/uart.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Table_Tennis_Detection.hex'
	arm-none-eabi-objcopy -O ihex "Table_Tennis_Detection.axf" "Table_Tennis_Detection.hex"
	@echo ' '

	@echo 'Building bin file: Table_Tennis_Detection.bin'
	arm-none-eabi-objcopy -O binary "Table_Tennis_Detection.axf" "Table_Tennis_Detection.bin"
	@echo ' '

	@echo 'Building s37 file: Table_Tennis_Detection.s37'
	arm-none-eabi-objcopy -O srec "Table_Tennis_Detection.axf" "Table_Tennis_Detection.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Table_Tennis_Detection.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Table_Tennis_Detection.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
