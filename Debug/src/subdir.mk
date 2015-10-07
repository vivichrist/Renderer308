################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Camera.cpp \
../src/Geometry.cpp \
../src/Loader.cpp \
../src/Plane.cpp \
../src/Shader.cpp \
../src/main.cpp 

OBJS += \
./src/Camera.o \
./src/Geometry.o \
./src/Loader.o \
./src/Plane.o \
./src/Shader.o \
./src/main.o 

CPP_DEPS += \
./src/Camera.d \
./src/Geometry.d \
./src/Loader.d \
./src/Plane.d \
./src/Shader.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I/usr/pkg/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


