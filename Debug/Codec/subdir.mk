################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Codec/cs43l22.c \
../Codec/stm32f4_discovery.c \
../Codec/stm32f4_discovery_audio.c 

OBJS += \
./Codec/cs43l22.o \
./Codec/stm32f4_discovery.o \
./Codec/stm32f4_discovery_audio.o 

C_DEPS += \
./Codec/cs43l22.d \
./Codec/stm32f4_discovery.d \
./Codec/stm32f4_discovery_audio.d 


# Each subdirectory must supply rules for building sources it contributes
Codec/%.o Codec/%.su Codec/%.cyclo: ../Codec/%.c Codec/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Codec -I../MP3Play -I../MP3Play/fixpt/pub -I../MP3Play/fixpt/real -I../AudioOut -I../Player -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Codec

clean-Codec:
	-$(RM) ./Codec/cs43l22.cyclo ./Codec/cs43l22.d ./Codec/cs43l22.o ./Codec/cs43l22.su ./Codec/stm32f4_discovery.cyclo ./Codec/stm32f4_discovery.d ./Codec/stm32f4_discovery.o ./Codec/stm32f4_discovery.su ./Codec/stm32f4_discovery_audio.cyclo ./Codec/stm32f4_discovery_audio.d ./Codec/stm32f4_discovery_audio.o ./Codec/stm32f4_discovery_audio.su

.PHONY: clean-Codec

