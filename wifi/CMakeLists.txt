add_executable(wifi
wifi.c
        )
# add_library(wifi wifi.h wifi.c)
# pull in common dependencies and additional pwm hardware support
target_link_libraries(wifi pico_stdlib hardware_pwm pico_netstack )
#target_include_directories(wifi PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
pico_add_extra_outputs(wifi)

pico_enable_stdio_usb(wifi 1)
# # create map/bin/hex file etc.
# pico_add_extra_outputs(sensor_driver)
# pico_enable_stdio_usb(sensor_driver 1)
# # add url via pico_set_program_url
# example_auto_set_url(sensor_driver)
