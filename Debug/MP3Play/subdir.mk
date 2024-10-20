################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MP3Play/MP3Play.c \
../MP3Play/WavPlay.c 

OBJS += \
./MP3Play/MP3Play.o \
./MP3Play/WavPlay.o 

C_DEPS += \
./MP3Play/MP3Play.d \
./MP3Play/WavPlay.d 


# Each subdirectory must supply rules for building sources it contributes
MP3Play/%.o MP3Play/%.su MP3Play/%.cyclo: ../MP3Play/%.c MP3Play/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Codec -I../MP3Play -I../MP3Play/fixpt/pub -I../MP3Play/fixpt/real -I../AudioOut -I../Player -I../aac/pub -I../aac/real -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MP3Play

clean-MP3Play:
	-$(RM) ./MP3Play/MP3Play.cyclo ./MP3Play/MP3Play.d ./MP3Play/MP3Play.o ./MP3Play/MP3Play.su ./MP3Play/WavPlay.cyclo ./MP3Play/WavPlay.d ./MP3Play/WavPlay.o ./MP3Play/WavPlay.su

.PHONY: clean-MP3Play

