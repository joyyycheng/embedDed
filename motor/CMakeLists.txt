# add_executable(motor
#         motor.c
#         motor.h
#         )
add_library(motor_driver motor_driver.h motor_driver.c)
# pull in common dependencies and additional pwm hardware support
target_link_libraries(motor_driver pico_stdlib hardware_pwm)

target_include_directories(motor_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# create map/bin/hex file etc.
# pico_add_extra_outputs(motor_driver)
# pico_enable_stdio_usb(motor_driver 1)
# # add url via pico_set_program_url
# example_auto_set_url(motor_driver)
