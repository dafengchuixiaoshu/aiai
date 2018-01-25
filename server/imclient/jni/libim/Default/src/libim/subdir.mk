
-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/libim/CConfigure.cpp \
../src/libim/CLog.cpp \
../src/libim/CIMI.cpp \
../src/libim/CObserver.cpp \
../src/libim/CObserverContainer.cpp \
../src/libim/BizCommon.cpp \
../src/libim/CWorker.cpp \
../src/libim/CWorkerContainer.cpp \
../src/libim/CWorkLogin.cpp \
../src/libim/CWorkMessage.cpp\
../src/libim/CWorkGroup.cpp \
../src/libim/CWorkContact.cpp \
../src/libim/CWorkUpdateTime.cpp

OBJS += \
./src/libim/CConfigure.o \
./src/libim/CLog.o \
./src/libim/CIMI.o \
./src/libim/CObserver.o \
./src/libim/CObserverContainer.o \
./src/libim/BizCommon.o \
./src/libim/CWorker.o \
./src/libim/CWorkerContainer.o \
./src/libim/CWorkLogin.o \
./src/libim/CWorkMessage.o\
./src/libim/CWorkGroup.o \
./src/libim/CWorkContact.o \
./src/libim/CWorkUpdateTime.o


CPP_DEPS += \
./src/libim/CConfigure.d \
./src/libim/CLog.d \
./src/libim/CIMI.d \
./src/libim/CObserver.d \
./src/libim/CObserverContainer.d \
./src/libim/BizCommon.d \
./src/libim/CWorker.d \
./src/libim/CWorkerContainer.d \
./src/libim/CWorkLogin.d \
./src/libim/CWorkMessage.d\
./src/libim/CWorkGroup.d \
./src/libim/CWorkContact.d \
./src/libim/CWorkUpdateTime.d


# Each subdirectory must supply rules for building sources it contributes
src/libim/%.o: ../src/libim/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


