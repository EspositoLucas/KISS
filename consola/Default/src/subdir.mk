################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/consola.c \
../src/consolaUtils.c 

OBJS += \
./src/consola.o \
./src/consolaUtils.o 

C_DEPS += \
./src/consola.d \
./src/consolaUtils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/media/sf_Shared/tp-2022-1c-Ubunteam/utils/include" -I"/media/sf_Shared/tp-2022-1c-Ubunteam/consola/include" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


