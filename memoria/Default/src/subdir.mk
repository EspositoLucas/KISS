################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/manejodetabla.c \
../src/memoria.c \
../src/swap.c 

OBJS += \
./src/manejodetabla.o \
./src/memoria.o \
./src/swap.o 

C_DEPS += \
./src/manejodetabla.d \
./src/memoria.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-Ubunteam/memoria/include" -I"/home/utnso/tp-2022-1c-Ubunteam/utils/include" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


