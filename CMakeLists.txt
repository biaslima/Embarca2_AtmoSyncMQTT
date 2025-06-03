
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(atmosync_mqtt C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(atmosync_mqtt main.c 
    lib/buzzer.c
    lib/matriz_led.c 
    lib/ssd1306.c 
    lib/modos.c 
    lib/setup.c
)

pico_set_program_name(atmosync_mqtt "atmosync_mqtt")
pico_set_program_version(atmosync_mqtt "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(atmosync_mqtt 0)
pico_enable_stdio_usb(atmosync_mqtt 1)

# Generate PIO header
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

# Add the standard library to the build
target_link_libraries(atmosync_mqtt
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
    pico_mbedtls
    pico_lwip_mbedtls
    hardware_gpio
    hardware_i2c
    hardware_pwm
    hardware_pio
    hardware_clocks
    )

# Add the standard include files to the build
target_include_directories(atmosync_mqtt PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(atmosync_mqtt 
        
        )

pico_add_extra_outputs(atmosync_mqtt)

