################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Player/AacPlayer.c \
../Player/MP3Play.c \
../Player/Player.c \
../Player/WavPlay.c 

OBJS += \
./Player/AacPlayer.o \
./Player/MP3Play.o \
./Player/Player.o \
./Player/WavPlay.o 

C_DEPS += \
./Player/AacPlayer.d \
./Player/MP3Play.d \
./Player/Player.d \
./Player/WavPlay.d 


# Each subdirectory must supply rules for building sources it contributes
Player/%.o Player/%.su Player/%.cyclo: ../Player/%.c Player/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Codec -I../MP3/fixpt/pub -I../MP3/fixpt/real -I../AudioOut -I../Player -I../aac/pub -I../aac/real -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Player

clean-Player:
	-$(RM) ./Player/AacPlayer.cyclo ./Player/AacPlayer.d ./Player/AacPlayer.o ./Player/AacPlayer.su ./Player/MP3Play.cyclo ./Player/MP3Play.d ./Player/MP3Play.o ./Player/MP3Play.su ./Player/Player.cyclo ./Player/Player.d ./Player/Player.o ./Player/Player.su ./Player/WavPlay.cyclo ./Player/WavPlay.d ./Player/WavPlay.o ./Player/WavPlay.su

.PHONY: clean-Player

