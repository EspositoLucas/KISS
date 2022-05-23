################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/kernel.c \
../src/kernelUtils.c \
../src/planificacion.c 

OBJS += \
./src/kernel.o \
./src/kernelUtils.o \
./src/planificacion.o 

C_DEPS += \
./src/kernel.d \
./src/kernelUtils.d \
./src/planificacion.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/media/sf_Shared/tp-2022-1c-Ubunteam/utils/include" -I"/media/sf_Shared/tp-2022-1c-Ubunteam/kernel/include" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


