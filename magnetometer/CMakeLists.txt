# add_executable( magnetometer_driver magnetometer_driver.c magnetometer_driver.h )
# target_link_libraries(magnetometer_driver pico_stdlib hardware_i2c)

# pico_enable_stdio_usb(magnetometer_driver 1)
# pico_enable_stdio_uart(magnetometer_driver 0)
# pico_add_extra_outputs(magnetometer_driver)


add_library(magnetometer_driver magnetometer_driver.c magnetometer_driver.h )
# pull in common dependencies and additional pwm hardware support
target_link_libraries(magnetometer_driver pico_stdlib hardware_i2c)
target_include_directories(magnetometer_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
