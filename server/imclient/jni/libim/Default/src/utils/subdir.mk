################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/utils/thread_pool.cpp \
../src/utils/EncodeUtil.cpp \
../src/utils/File.cpp \
../src/utils/NativeCrashHandler.cpp

OBJS += \
./src/utils/thread_pool.o \
./src/utils/EncodeUtil.o \
./src/utils/File.o \
./src/utils/NativeCrashHandler.o

CPP_DEPS += \
./src/utils/thread_pool.d \
./src/utils/EncodeUtil.d \
./src/utils/File.d \
./src/utils/NativeCrashHandler.d

# Each subdirectory must supply rules for building sources it contributes
src/utils/%.o: ../src/utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


