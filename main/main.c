/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"


#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"

#include "motor_driver.h"
#include "wheelEncoder_driver.h"
#include "ultrasonic_driver.h"

// lab 5 for message buffer
#include "message_buffer.h"

#define WHEELENCODER_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif


#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

// message buffer
#define mbaTASK_MESSAGE_BUFFER_SIZE (60)



void motor_task(__unused void *params)
{
    
    while(1)
    {            
        moveBackward(); 
    }
}

void wheelencoder_task(__unused void *params)
{
 
    while(1)
    {
        int pinState1 = getResults1(); 
        getSpeed(pinState1);
    }

}

void ultrasonic_task(__unused void *params)
{
    while(1)
    {
        uint64_t pulseLength = get_measurement_cm();
        printf("this is distance: %lldcm \n", pulseLength);
    }
}

void barcode_task(__unused void *params)
{
    
    struct repeating_timer timer;

    while(1)
    {
        char character = getchar();
        printf("%c", character);
        switch (character) {
            case 'r': {
                // Call the custom reset function
                //reset_function();
                add_repeating_timer_ms(-100, adc_callback, NULL, &timer); //Value determines how often ADC value is printed 
                break;
            }
        }
    }
}


void vLaunch( void) {


    TaskHandle_t motorTask;
    xTaskCreate(motor_task, "MotorThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &motorTask);
    
    TaskHandle_t wheelencodertask;
    xTaskCreate(wheelencoder_task, "WheelEncoderThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &wheelencodertask);

    TaskHandle_t ultrasonictask;
    xTaskCreate(ultrasonic_task, "UltraSonicThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY + 1, &ultrasonictask);

    TaskHandle_t barcodetask;
    xTaskCreate(barcode_task, "UltraSonicThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY , &barcodetask);
    
    

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main( void )
{
    stdio_init_all();
    motorInit();
    sensorInit();
    ultrasonic_init();
    barcodeInit();

    sleep_ms(5000);

    /* Configure the hardware ready to run the demo. */
    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif ( RUN_FREERTOS_ON_CORE == 1 )
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}
