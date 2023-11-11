################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MP3Play/fixpt/mp3dec.c \
../MP3Play/fixpt/mp3tabs.c 

OBJS += \
./MP3Play/fixpt/mp3dec.o \
./MP3Play/fixpt/mp3tabs.o 

C_DEPS += \
./MP3Play/fixpt/mp3dec.d \
./MP3Play/fixpt/mp3tabs.d 


# Each subdirectory must supply rules for building sources it contributes
MP3Play/fixpt/%.o MP3Play/fixpt/%.su MP3Play/fixpt/%.cyclo: ../MP3Play/fixpt/%.c MP3Play/fixpt/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Codec -I../MP3Play -I../MP3Play/fixpt/pub -I../MP3Play/fixpt/real -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MP3Play-2f-fixpt

clean-MP3Play-2f-fixpt:
	-$(RM) ./MP3Play/fixpt/mp3dec.cyclo ./MP3Play/fixpt/mp3dec.d ./MP3Play/fixpt/mp3dec.o ./MP3Play/fixpt/mp3dec.su ./MP3Play/fixpt/mp3tabs.cyclo ./MP3Play/fixpt/mp3tabs.d ./MP3Play/fixpt/mp3tabs.o ./MP3Play/fixpt/mp3tabs.su

.PHONY: clean-MP3Play-2f-fixpt

