################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/kernel.c \
../src/planificacion.c \
../src/planificacionUtils.c 

OBJS += \
./src/kernel.o \
./src/planificacion.o \
./src/planificacionUtils.o 

C_DEPS += \
./src/kernel.d \
./src/planificacion.d \
./src/planificacionUtils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-Ubunteam/kernel/include" -I"/home/utnso/tp-2022-1c-Ubunteam/utils/include" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


