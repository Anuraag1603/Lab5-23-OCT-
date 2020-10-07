################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/Debounce.c \
../Sources/Events.c \
../Sources/FIFO.c \
../Sources/FTM.c \
../Sources/Flash.c \
../Sources/Game.c \
../Sources/I2C.c \
../Sources/LEDS.c \
../Sources/PIT.c \
../Sources/RNG.c \
../Sources/RTC.c \
../Sources/TSI.c \
../Sources/UART.c \
../Sources/accel.c \
../Sources/main.c \
../Sources/median.c \
../Sources/packet.c 

OBJS += \
./Sources/Debounce.o \
./Sources/Events.o \
./Sources/FIFO.o \
./Sources/FTM.o \
./Sources/Flash.o \
./Sources/Game.o \
./Sources/I2C.o \
./Sources/LEDS.o \
./Sources/PIT.o \
./Sources/RNG.o \
./Sources/RTC.o \
./Sources/TSI.o \
./Sources/UART.o \
./Sources/accel.o \
./Sources/main.o \
./Sources/median.o \
./Sources/packet.o 

C_DEPS += \
./Sources/Debounce.d \
./Sources/Events.d \
./Sources/FIFO.d \
./Sources/FTM.d \
./Sources/Flash.d \
./Sources/Game.d \
./Sources/I2C.d \
./Sources/LEDS.d \
./Sources/PIT.d \
./Sources/RNG.d \
./Sources/RTC.d \
./Sources/TSI.d \
./Sources/UART.d \
./Sources/accel.d \
./Sources/main.d \
./Sources/median.d \
./Sources/packet.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"H:/2015 Spring/Embedded Software/Lab5 (New)/Static_Code/PDD" -I"H:/2015 Spring/Embedded Software/Lab5 (New)/Static_Code/IO_Map" -I"H:/2015 Spring/Embedded Software/Lab5 (New)/Sources" -I"H:/2015 Spring/Embedded Software/Lab5 (New)/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


