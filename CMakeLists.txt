
# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)
set(PICO_BOARD pico_w)
set(PROJECT_NAME "tunel-io")
#add_compile_definitions(PICO_CYW43_ARCH_POLL)
#set(WIFI_SSID abc)
#set(WIFI_PASSWORD abc)
# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()



# Add a compilation subdirectory
add_subdirectory(picolcd/Config)
add_subdirectory(picolcd/Infrared)
add_subdirectory(picolcd/Icm20948)
add_subdirectory(picolcd/LCD)

# Add a header directory
include_directories(./picolcd/Config)
include_directories(./picolcd/Icm20948)
include_directories(./picolcd/LCD)
include_directories(./picolcd/Infrared)


add_subdirectory(ugui)
include_directories(./ugui)


include_directories(./)
# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.c
    uart_irq.c
    tcp_server.c
    lcd.c
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
        pico_cyw43_arch_lwip_threadsafe_background
        #pico_cyw43_arch_lwip_poll
        pico_stdlib
        pico_lwip
    #hardware_flash
    #hardware_irq
    #hardware_adc
    hardware_pwm
    hardware_i2c
    hardware_spi
    #hardware_dma
    #hardware_exception
    LCD Infrared 
    Icm20948 
    Config
    ugui
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
