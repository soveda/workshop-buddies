cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(bib_control_rate_test C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
pico_sdk_init()

add_executable(bib_control_rate_test bib.cpp)
target_include_directories(bib_control_rate_test PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(bib_control_rate_test
    pico_unique_id pico_stdlib hardware_dma hardware_i2c hardware_pwm
    hardware_adc hardware_spi)

# Keeping all audio code in RAM avoids unpredictable flash-cache stalls in
# ComputerCard's 48 kHz interrupt.
pico_set_binary_type(bib_control_rate_test copy_to_ram)
pico_add_extra_outputs(bib_control_rate_test)
target_compile_definitions(bib_control_rate_test PRIVATE PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64)
pico_enable_stdio_usb(bib_control_rate_test 0)
target_compile_options(bib_control_rate_test PRIVATE -O3 -Wdouble-promotion -Wfloat-conversion -Wall -Wextra)
target_link_options(bib_control_rate_test PRIVATE -Wl,--print-memory-usage)
