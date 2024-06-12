cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME fancontrol)
include("cmake/gcc-arm-none-eabi.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

enable_language(C ASM)

project(${CMAKE_PROJECT_NAME})

add_executable(${CMAKE_PROJECT_NAME} Core/Src/main.c main.cc)

add_subdirectory(cmake/stm32cubemx)
target_link_libraries(${CMAKE_PROJECT_NAME} stm32cubemx celsius_table)
add_dependencies(${CMAKE_PROJECT_NAME} celsius_table)

add_custom_target(flash
  COMMAND ${STM32_CUBE_PROGRAMMER} -c port=SWD -w ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf
  COMMENT "Flashing application to the device"
  DEPENDS ${CMAKE_PROJECT_NAME}.elf
)

# run generate_table.py to generate adc_to_celsius.h
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/adc_to_celsius.h ${CMAKE_CURRENT_BINARY_DIR}/adc_to_celsius.c
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_table.py
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_table.py
  COMMENT "Generating adc_to_celsius.h"
)
add_library(celsius_table ${CMAKE_CURRENT_BINARY_DIR}/adc_to_celsius.c)
target_include_directories(celsius_table INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
