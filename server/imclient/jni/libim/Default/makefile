################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/zlib-1.2.5/subdir.mk
-include src/xmpp/subdir.mk
-include src/xcap/subdir.mk
-include src/sqlite/subdir.mk
-include src/utils/subdir.mk
-include src/libim/subdir.mk
-include src/libclient/subdir.mk
-include src/protobuf/libprotobuf/subdir.mk
-include src/protobuf/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
#ifneq ($(strip $(CC_DEPS)),)
#-include $(CC_DEPS)
#endif
#ifneq ($(strip $(C++_DEPS)),)
#-include $(C++_DEPS)
#endif
#ifneq ($(strip $(C_UPPER_DEPS)),)
#-include $(C_UPPER_DEPS)
#endif
#ifneq ($(strip $(CXX_DEPS)),)
#-include $(CXX_DEPS)
#endif
#ifneq ($(strip $(CPP_DEPS)),)
#-include $(CPP_DEPS)
#endif
#ifneq ($(strip $(C_DEPS)),)
#-include $(C_DEPS)
#endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libim

# Tool invocations
libim: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cygwin C++ Linker'
	g++  -o "libim" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(CPP_DEPS)$(C_DEPS) libim
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
