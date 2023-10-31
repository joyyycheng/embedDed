# add_executable(motor
#         motor.c
#         motor.h
#         )
add_library(barcode_driver barcode_driver.h barcode_driver.c)
# pull in common dependencies and additional pwm hardware support
target_link_libraries(barcode_driver pico_stdlib hardware_pwm hardware_adc)

target_include_directories(barcode_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# create map/bin/hex file etc.
# pico_add_extra_outputs(motor_driver)
# pico_enable_stdio_usb(motor_driver 1)
# # add url via pico_set_program_url
# example_auto_set_url(motor_driver)
