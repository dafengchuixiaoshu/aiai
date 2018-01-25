################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include $(PWD)/base.mk

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protobuf/libprotobuf/coded_stream.cc \
../src/protobuf/libprotobuf/common.cc \
../src/protobuf/libprotobuf/descriptor.cc \
../src/protobuf/libprotobuf/descriptor.pb.cc \
../src/protobuf/libprotobuf/descriptor_database.cc \
../src/protobuf/libprotobuf/dynamic_message.cc \
../src/protobuf/libprotobuf/extension_set.cc \
../src/protobuf/libprotobuf/extension_set_heavy.cc \
../src/protobuf/libprotobuf/generated_message_reflection.cc \
../src/protobuf/libprotobuf/generated_message_util.cc \
../src/protobuf/libprotobuf/gzip_stream.cc \
../src/protobuf/libprotobuf/importer.cc \
../src/protobuf/libprotobuf/message.cc \
../src/protobuf/libprotobuf/message_lite.cc \
../src/protobuf/libprotobuf/once.cc \
../src/protobuf/libprotobuf/parser.cc \
../src/protobuf/libprotobuf/printer.cc \
../src/protobuf/libprotobuf/reflection_ops.cc \
../src/protobuf/libprotobuf/repeated_field.cc \
../src/protobuf/libprotobuf/service.cc \
../src/protobuf/libprotobuf/stringprintf.cc \
../src/protobuf/libprotobuf/structurally_valid.cc \
../src/protobuf/libprotobuf/strutil.cc \
../src/protobuf/libprotobuf/substitute.cc \
../src/protobuf/libprotobuf/text_format.cc \
../src/protobuf/libprotobuf/tokenizer.cc \
../src/protobuf/libprotobuf/unknown_field_set.cc \
../src/protobuf/libprotobuf/wire_format.cc \
../src/protobuf/libprotobuf/wire_format_lite.cc \
../src/protobuf/libprotobuf/zero_copy_stream.cc \
../src/protobuf/libprotobuf/zero_copy_stream_impl.cc \
../src/protobuf/libprotobuf/zero_copy_stream_impl_lite.cc \
../src/protobuf/libprotobuf/atomicops_internals_arm_gcc.cc

C_SRCS += 

OBJS += \
./src/protobuf/libprotobuf/coded_stream.o \
./src/protobuf/libprotobuf/common.o \
./src/protobuf/libprotobuf/descriptor.o \
./src/protobuf/libprotobuf/descriptor.pb.o \
./src/protobuf/libprotobuf/descriptor_database.o \
./src/protobuf/libprotobuf/dynamic_message.o \
./src/protobuf/libprotobuf/extension_set.o \
./src/protobuf/libprotobuf/extension_set_heavy.o \
./src/protobuf/libprotobuf/generated_message_reflection.o \
./src/protobuf/libprotobuf/generated_message_util.o \
./src/protobuf/libprotobuf/gzip_stream.o \
./src/protobuf/libprotobuf/importer.o \
./src/protobuf/libprotobuf/message.o \
./src/protobuf/libprotobuf/message_lite.o \
./src/protobuf/libprotobuf/once.o \
./src/protobuf/libprotobuf/parser.o \
./src/protobuf/libprotobuf/printer.o \
./src/protobuf/libprotobuf/reflection_ops.o \
./src/protobuf/libprotobuf/repeated_field.o \
./src/protobuf/libprotobuf/service.o \
./src/protobuf/libprotobuf/stringprintf.o \
./src/protobuf/libprotobuf/structurally_valid.o \
./src/protobuf/libprotobuf/strutil.o \
./src/protobuf/libprotobuf/substitute.o \
./src/protobuf/libprotobuf/text_format.o \
./src/protobuf/libprotobuf/tokenizer.o \
./src/protobuf/libprotobuf/unknown_field_set.o \
./src/protobuf/libprotobuf/wire_format.o \
./src/protobuf/libprotobuf/wire_format_lite.o \
./src/protobuf/libprotobuf/zero_copy_stream.o \
./src/protobuf/libprotobuf/zero_copy_stream_impl.o \
./src/protobuf/libprotobuf/zero_copy_stream_impl_lite.o \
./src/protobuf/libprotobuf/atomicops_internals_arm_gcc.o

CPP_DEPS += \
./src/protobuf/libprotobuf/coded_stream.d \
./src/protobuf/libprotobuf/common.d \
./src/protobuf/libprotobuf/descriptor.d \
./src/protobuf/libprotobuf/descriptor.pb.d \
./src/protobuf/libprotobuf/descriptor_database.d \
./src/protobuf/libprotobuf/dynamic_message.d \
./src/protobuf/libprotobuf/extension_set.d \
./src/protobuf/libprotobuf/extension_set_heavy.d \
./src/protobuf/libprotobuf/generated_message_reflection.d \
./src/protobuf/libprotobuf/generated_message_util.d \
./src/protobuf/libprotobuf/gzip_stream.d \
./src/protobuf/libprotobuf/importer.d \
./src/protobuf/libprotobuf/message.d \
./src/protobuf/libprotobuf/message_lite.d \
./src/protobuf/libprotobuf/once.d \
./src/protobuf/libprotobuf/parser.d \
./src/protobuf/libprotobuf/printer.d \
./src/protobuf/libprotobuf/reflection_ops.d \
./src/protobuf/libprotobuf/repeated_field.d \
./src/protobuf/libprotobuf/service.d \
./src/protobuf/libprotobuf/stringprintf.d \
./src/protobuf/libprotobuf/structurally_valid.d \
./src/protobuf/libprotobuf/strutil.d \
./src/protobuf/libprotobuf/substitute.d \
./src/protobuf/libprotobuf/text_format.d \
./src/protobuf/libprotobuf/tokenizer.d \
./src/protobuf/libprotobuf/unknown_field_set.d \
./src/protobuf/libprotobuf/wire_format.d \
./src/protobuf/libprotobuf/wire_format_lite.d \
./src/protobuf/libprotobuf/zero_copy_stream.d \
./src/protobuf/libprotobuf/zero_copy_stream_impl.d \
./src/protobuf/libprotobuf/zero_copy_stream_impl_lite.d \
./src/protobuf/libprotobuf/atomicops_internals_arm_gcc.d

C_DEPS += 

# Each subdirectory must supply rules for building sources it contributes
src/protobuf/libprotobuf/%.o: ../src/protobuf/libprotobuf/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/protobuf/libprotobuf/%.o: ../src/protobuf/libprotobuf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc $(BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


