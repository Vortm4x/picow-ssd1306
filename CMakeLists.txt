cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w)
include(pico_sdk_import.cmake)

project(pico-ssd1306-driver C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(lwip_import.cmake)

pico_sdk_init()


add_executable(${PROJECT_NAME}
    src/ssd1306_driver.h src/ssd1306_driver.c
    examples/main.c
    examples/raspberry26x32.h
    examples/ssd1306_font.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "include/"
    "src/"
    "examples/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
    hardware_i2c
    pico_cyw43_arch_none
    LWIP_PORT
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

# Run the entire project in SRAM
# pico_set_binary_type(pico-freertos copy_to_ram)

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)