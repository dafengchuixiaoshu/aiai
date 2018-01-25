################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/xmpp/CXmppStack.cpp

OBJS += \
./src/xmpp/CXmppStack.o

CPP_DEPS += \
./src/xmpp/CXmppStack.d

# Each subdirectory must supply rules for building sources it contributes
src/xmpp/%.o: ../src/xmpp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


