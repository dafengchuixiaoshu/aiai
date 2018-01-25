################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/libclient/base64.cpp \
../src/libclient/immodule.cpp \
../src/libclient/immsg.cpp \
../src/libclient/imsocket.cpp \
../src/libclient/httpsocket.cpp \
../src/libclient/protobyte.cpp \
../src/libclient/protoxml.cpp \
../src/libclient/thread.cpp 

C_SRCS += 

OBJS += \
./src/libclient/immodule.o \
./src/libclient/base64.o \
./src/libclient/immsg.o \
./src/libclient/imsocket.o \
./src/libclient/httpsocket.o \
./src/libclient/protobyte.o \
./src/libclient/protoxml.o \
./src/libclient/thread.o 

CPP_DEPS += \
./src/libclient/immodule.d \
./src/libclient/base64.d \
./src/libclient/immsg.d \
./src/libclient/imsocket.d \
./src/libclient/httpsocket.d \
./src/libclient/protobyte.d \
./src/libclient/protoxml.d \
./src/libclient/thread.d 

C_DEPS += 

# Each subdirectory must supply rules for building sources it contributes
src/libclient/%.o: ../src/libclient/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/libclient/%.o: ../src/libclient/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


