################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/Eeprom/eeprom.c \
../Middleware/Eeprom/eeprom_test.c \
../Middleware/Eeprom/eeprom_vars.c 

OBJS += \
./Middleware/Eeprom/eeprom.o \
./Middleware/Eeprom/eeprom_test.o \
./Middleware/Eeprom/eeprom_vars.o 

C_DEPS += \
./Middleware/Eeprom/eeprom.d \
./Middleware/Eeprom/eeprom_test.d \
./Middleware/Eeprom/eeprom_vars.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/Eeprom/%.o Middleware/Eeprom/%.su Middleware/Eeprom/%.cyclo: ../Middleware/Eeprom/%.c Middleware/Eeprom/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-Eeprom

clean-Middleware-2f-Eeprom:
	-$(RM) ./Middleware/Eeprom/eeprom.cyclo ./Middleware/Eeprom/eeprom.d ./Middleware/Eeprom/eeprom.o ./Middleware/Eeprom/eeprom.su ./Middleware/Eeprom/eeprom_test.cyclo ./Middleware/Eeprom/eeprom_test.d ./Middleware/Eeprom/eeprom_test.o ./Middleware/Eeprom/eeprom_test.su ./Middleware/Eeprom/eeprom_vars.cyclo ./Middleware/Eeprom/eeprom_vars.d ./Middleware/Eeprom/eeprom_vars.o ./Middleware/Eeprom/eeprom_vars.su

.PHONY: clean-Middleware-2f-Eeprom

