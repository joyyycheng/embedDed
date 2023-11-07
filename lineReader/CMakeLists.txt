# add_executable(algorithm
#         algorithm.c
#         )

# target_link_libraries(algorithm pico_stdlib hardware_pwm hardware_adc)

# # create map/bin/hex file etc.
# pico_add_extra_outputs(algorithm)

# pico_enable_stdio_usb(algorithm 1)

# # add url via pico_set_program_url
# example_auto_set_url(algorithm)



add_library(linereader_driver linereader_driver.c linereader_driver.h )
# pull in common dependencies and additional pwm hardware support
target_link_libraries(linereader_driver pico_stdlib hardware_pwm hardware_adc)
target_include_directories(linereader_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
