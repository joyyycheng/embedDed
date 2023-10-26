add_executable(mainCodes 
        mainCodes.h
        mainCodes.c
        )



# pull in common dependencies and additional pwm hardware support
target_link_libraries(mainCodes wheelEncoder_driver motor_driver ultrasonic_driver pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(mainCodes)
pico_enable_stdio_usb(mainCodes 1)
# add url via pico_set_program_url
example_auto_set_url(mainCodes)
