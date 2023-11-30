add_library(barcode_driver barcode_driver.h barcode_driver.c)

target_link_libraries(barcode_driver pico_stdlib hardware_pwm hardware_adc)

target_include_directories(barcode_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
