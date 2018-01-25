################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sqlite/DBUpdate.cpp \
../src/sqlite/SqliteImpl.cpp 

C_SRCS += \
../src/sqlite/sqlite3.c 

OBJS += \
./src/sqlite/DBUpdate.o \
./src/sqlite/SqliteImpl.o \
./src/sqlite/sqlite3.o 

CPP_DEPS += \
./src/sqlite/DBUpdate.d \
./src/sqlite/SqliteImpl.d 

C_DEPS += \
./src/sqlite/sqlite3.d 


# Each subdirectory must supply rules for building sources it contributes
src/sqlite/%.o: ../src/sqlite/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/sqlite/%.o: ../src/sqlite/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


