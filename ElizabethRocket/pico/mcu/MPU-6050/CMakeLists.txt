cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(MPU-6050)

pico_sdk_init()

add_executable(MPU-6050
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

target_link_libraries(MPU-6050
    pico_stdlib
    hardware_i2c
    m
)

pico_set_linker_script(MPU-6050 ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld)

pico_add_extra_outputs(MPU-6050)

pico_enable_stdio_usb(MPU-6050 1)
pico_enable_stdio_uart(MPU-6050 0)