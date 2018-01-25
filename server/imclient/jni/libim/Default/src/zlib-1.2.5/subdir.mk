################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/zlib-1.2.5/adler32.c \
../src/zlib-1.2.5/compress.c \
../src/zlib-1.2.5/crc32.c \
../src/zlib-1.2.5/deflate.c \
../src/zlib-1.2.5/gzclose.c \
../src/zlib-1.2.5/gzlib.c \
../src/zlib-1.2.5/gzread.c \
../src/zlib-1.2.5/gzwrite.c \
../src/zlib-1.2.5/infback.c \
../src/zlib-1.2.5/inffast.c \
../src/zlib-1.2.5/inflate.c \
../src/zlib-1.2.5/inftrees.c \
../src/zlib-1.2.5/trees.c \
../src/zlib-1.2.5/uncompr.c \
../src/zlib-1.2.5/zutil.c 

OBJS += \
./src/zlib-1.2.5/adler32.o \
./src/zlib-1.2.5/compress.o \
./src/zlib-1.2.5/crc32.o \
./src/zlib-1.2.5/deflate.o \
./src/zlib-1.2.5/gzclose.o \
./src/zlib-1.2.5/gzlib.o \
./src/zlib-1.2.5/gzread.o \
./src/zlib-1.2.5/gzwrite.o \
./src/zlib-1.2.5/infback.o \
./src/zlib-1.2.5/inffast.o \
./src/zlib-1.2.5/inflate.o \
./src/zlib-1.2.5/inftrees.o \
./src/zlib-1.2.5/trees.o \
./src/zlib-1.2.5/uncompr.o \
./src/zlib-1.2.5/zutil.o 

C_DEPS += \
./src/zlib-1.2.5/adler32.d \
./src/zlib-1.2.5/compress.d \
./src/zlib-1.2.5/crc32.d \
./src/zlib-1.2.5/deflate.d \
./src/zlib-1.2.5/gzclose.d \
./src/zlib-1.2.5/gzlib.d \
./src/zlib-1.2.5/gzread.d \
./src/zlib-1.2.5/gzwrite.d \
./src/zlib-1.2.5/infback.d \
./src/zlib-1.2.5/inffast.d \
./src/zlib-1.2.5/inflate.d \
./src/zlib-1.2.5/inftrees.d \
./src/zlib-1.2.5/trees.d \
./src/zlib-1.2.5/uncompr.d \
./src/zlib-1.2.5/zutil.d 


# Each subdirectory must supply rules for building sources it contributes
src/zlib-1.2.5/%.o: ../src/zlib-1.2.5/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


