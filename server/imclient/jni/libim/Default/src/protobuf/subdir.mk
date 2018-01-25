################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protobuf/im.pb.cc  

C_SRCS += 

OBJS += \
./src/protobuf/im.pb.o 

CPP_DEPS += \
./src/protobuf/im.pb.d 

C_DEPS += 

# Each subdirectory must supply rules for building sources it contributes
src/protobuf/%.o: ../src/protobuf/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/protobuf/%.o: ../src/protobuf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


