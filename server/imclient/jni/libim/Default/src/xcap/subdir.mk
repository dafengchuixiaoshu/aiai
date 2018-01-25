################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/xcap/AutoBuffer.cpp \
../src/xcap/Markup.cpp \
../src/xcap/RequestBuilder.cpp \
../src/xcap/RequestBuilder_main2.cpp \
../src/xcap/ResponseProcessor_main2.cpp \
../src/xcap/XCapCore.cpp \


OBJS += \
./src/xcap/AutoBuffer.o \
./src/xcap/Markup.o \
./src/xcap/RequestBuilder.o \
./src/xcap/RequestBuilder_main2.o \
./src/xcap/ResponseProcessor_main2.o \
./src/xcap/XCapCore.o \

CPP_DEPS += \
./src/xcap/AutoBuffer.d \
./src/xcap/Markup.d \
./src/xcap/RequestBuilder.d \
./src/xcap/RequestBuilder_main2.d \
./src/xcap/ResponseProcessor_main2.d \
./src/xcap/XCapCore.d \



# Each subdirectory must supply rules for building sources it contributes
src/xcap/%.o: ../src/xcap/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


