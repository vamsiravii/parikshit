################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/EEPROM/eeprom_driver.c 

OBJS += \
./Drivers/EEPROM/eeprom_driver.o 

C_DEPS += \
./Drivers/EEPROM/eeprom_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/EEPROM/%.o Drivers/EEPROM/%.su Drivers/EEPROM/%.cyclo: ../Drivers/EEPROM/%.c Drivers/EEPROM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-EEPROM

clean-Drivers-2f-EEPROM:
	-$(RM) ./Drivers/EEPROM/eeprom_driver.cyclo ./Drivers/EEPROM/eeprom_driver.d ./Drivers/EEPROM/eeprom_driver.o ./Drivers/EEPROM/eeprom_driver.su

.PHONY: clean-Drivers-2f-EEPROM

