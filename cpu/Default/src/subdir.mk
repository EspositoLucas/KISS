################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cpu.c \
../src/mmu.c \
../src/tlb.c 

OBJS += \
./src/cpu.o \
./src/mmu.o \
./src/tlb.o 

C_DEPS += \
./src/cpu.d \
./src/mmu.d \
./src/tlb.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-Ubunteam/cpu/include" -I"/home/utnso/tp-2022-1c-Ubunteam/utils/include" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


