# add_executable(sensor
# sensor.c
#         )
add_library(ultrasonic_driver ultrasonic_driver.h ultrasonic_driver.c)
# pull in common dependencies and additional pwm hardware support
target_link_libraries(ultrasonic_driver pico_stdlib hardware_pwm)
target_include_directories(ultrasonic_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# # create map/bin/hex file etc.
# pico_add_extra_outputs(sensor_driver)
# pico_enable_stdio_usb(sensor_driver 1)
# # add url via pico_set_program_url
# example_auto_set_url(sensor_driver)
