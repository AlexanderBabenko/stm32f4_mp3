################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MP3Play/fixpt/real/bitstream.c \
../MP3Play/fixpt/real/buffers.c \
../MP3Play/fixpt/real/dct32.c \
../MP3Play/fixpt/real/dequant.c \
../MP3Play/fixpt/real/dqchan.c \
../MP3Play/fixpt/real/huffman.c \
../MP3Play/fixpt/real/hufftabs.c \
../MP3Play/fixpt/real/imdct.c \
../MP3Play/fixpt/real/polyphase.c \
../MP3Play/fixpt/real/scalfact.c \
../MP3Play/fixpt/real/stproc.c \
../MP3Play/fixpt/real/subband.c \
../MP3Play/fixpt/real/trigtabs.c 

OBJS += \
./MP3Play/fixpt/real/bitstream.o \
./MP3Play/fixpt/real/buffers.o \
./MP3Play/fixpt/real/dct32.o \
./MP3Play/fixpt/real/dequant.o \
./MP3Play/fixpt/real/dqchan.o \
./MP3Play/fixpt/real/huffman.o \
./MP3Play/fixpt/real/hufftabs.o \
./MP3Play/fixpt/real/imdct.o \
./MP3Play/fixpt/real/polyphase.o \
./MP3Play/fixpt/real/scalfact.o \
./MP3Play/fixpt/real/stproc.o \
./MP3Play/fixpt/real/subband.o \
./MP3Play/fixpt/real/trigtabs.o 

C_DEPS += \
./MP3Play/fixpt/real/bitstream.d \
./MP3Play/fixpt/real/buffers.d \
./MP3Play/fixpt/real/dct32.d \
./MP3Play/fixpt/real/dequant.d \
./MP3Play/fixpt/real/dqchan.d \
./MP3Play/fixpt/real/huffman.d \
./MP3Play/fixpt/real/hufftabs.d \
./MP3Play/fixpt/real/imdct.d \
./MP3Play/fixpt/real/polyphase.d \
./MP3Play/fixpt/real/scalfact.d \
./MP3Play/fixpt/real/stproc.d \
./MP3Play/fixpt/real/subband.d \
./MP3Play/fixpt/real/trigtabs.d 


# Each subdirectory must supply rules for building sources it contributes
MP3Play/fixpt/real/%.o MP3Play/fixpt/real/%.su MP3Play/fixpt/real/%.cyclo: ../MP3Play/fixpt/real/%.c MP3Play/fixpt/real/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Codec -I../MP3Play -I../MP3Play/fixpt/pub -I../MP3Play/fixpt/real -I../AudioOut -I../Player -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MP3Play-2f-fixpt-2f-real

clean-MP3Play-2f-fixpt-2f-real:
	-$(RM) ./MP3Play/fixpt/real/bitstream.cyclo ./MP3Play/fixpt/real/bitstream.d ./MP3Play/fixpt/real/bitstream.o ./MP3Play/fixpt/real/bitstream.su ./MP3Play/fixpt/real/buffers.cyclo ./MP3Play/fixpt/real/buffers.d ./MP3Play/fixpt/real/buffers.o ./MP3Play/fixpt/real/buffers.su ./MP3Play/fixpt/real/dct32.cyclo ./MP3Play/fixpt/real/dct32.d ./MP3Play/fixpt/real/dct32.o ./MP3Play/fixpt/real/dct32.su ./MP3Play/fixpt/real/dequant.cyclo ./MP3Play/fixpt/real/dequant.d ./MP3Play/fixpt/real/dequant.o ./MP3Play/fixpt/real/dequant.su ./MP3Play/fixpt/real/dqchan.cyclo ./MP3Play/fixpt/real/dqchan.d ./MP3Play/fixpt/real/dqchan.o ./MP3Play/fixpt/real/dqchan.su ./MP3Play/fixpt/real/huffman.cyclo ./MP3Play/fixpt/real/huffman.d ./MP3Play/fixpt/real/huffman.o ./MP3Play/fixpt/real/huffman.su ./MP3Play/fixpt/real/hufftabs.cyclo ./MP3Play/fixpt/real/hufftabs.d ./MP3Play/fixpt/real/hufftabs.o ./MP3Play/fixpt/real/hufftabs.su ./MP3Play/fixpt/real/imdct.cyclo ./MP3Play/fixpt/real/imdct.d ./MP3Play/fixpt/real/imdct.o ./MP3Play/fixpt/real/imdct.su ./MP3Play/fixpt/real/polyphase.cyclo ./MP3Play/fixpt/real/polyphase.d ./MP3Play/fixpt/real/polyphase.o ./MP3Play/fixpt/real/polyphase.su ./MP3Play/fixpt/real/scalfact.cyclo ./MP3Play/fixpt/real/scalfact.d ./MP3Play/fixpt/real/scalfact.o ./MP3Play/fixpt/real/scalfact.su ./MP3Play/fixpt/real/stproc.cyclo ./MP3Play/fixpt/real/stproc.d ./MP3Play/fixpt/real/stproc.o ./MP3Play/fixpt/real/stproc.su ./MP3Play/fixpt/real/subband.cyclo ./MP3Play/fixpt/real/subband.d ./MP3Play/fixpt/real/subband.o ./MP3Play/fixpt/real/subband.su ./MP3Play/fixpt/real/trigtabs.cyclo ./MP3Play/fixpt/real/trigtabs.d ./MP3Play/fixpt/real/trigtabs.o ./MP3Play/fixpt/real/trigtabs.su

.PHONY: clean-MP3Play-2f-fixpt-2f-real

