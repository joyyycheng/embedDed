if (NOT FREERTOS_KERNEL_PATH AND NOT DEFINED ENV{FREERTOS_KERNEL_PATH})
    message("Skipping Pico W FreeRTOS examples as FREERTOS_KERNEL_PATH not defined")
else()
    include(FreeRTOS_Kernel_import.cmake)

    add_subdirectory(main)
    add_subdirectory(motor)
    add_subdirectory(wheelEncoder)
    add_subdirectory(ultrasonic)
    add_subdirectory(wifi)
    add_subdirectory(barcode)
    add_subdirectory(magnetometer)
    add_subdirectory(pidcontroller)
    add_subdirectory(lineReader)
endif()
