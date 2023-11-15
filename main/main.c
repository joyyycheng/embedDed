/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"
#include <math.h>
#include "motor_driver.h"
#include "wheelEncoder_driver.h"
#include "ultrasonic_driver.h"
#include "barcode_driver.h"
#include "pidcontroller.h"
#include "magnetometer_driver.h"
#include "linereader_driver.h"

// lab 5 for message buffer
#include "message_buffer.h"
#define WHEELENCODER_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif
#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

// message buffer
#define mbaTASK_MESSAGE_BUFFER_SIZE (60)

//Initialize initial = 0  // Start with the initial heading
//Set amount = 10  // Specify the degree of rotation for each step
//function rotateLeft():
//  heading = (initial - amount) % 360

void rotate_left(float rotate_amount, float initial_heading)
{
  float final_heading = fmod((initial_heading - rotate_amount), 360.0);

    while (1)
    {
    float current_heading = magnetometer_heading();

    printf("staring angle: %f | ending angle: %f | current angle: %f\n", initial_heading, final_heading, current_heading);

    // vTaskDelay(pdMS_TO_TICKS(100));


    if (current_heading > final_heading - 10 && current_heading < final_heading + 10)
    {
      // Angle has reached or exceeded 90 degrees, stop the car
      // printf("Reached 90 degrees, stopping the car\n");
      stop();
      vTaskDelay(pdMS_TO_TICKS(100));
      // moveForward();
      // gas();
      break; // Exit the loop and end the task
    }
    else
    {
      turnLeft();
      gas();
    }
  }
}

void rotate_right(double rotate_amount, double initial_heading)
{
  float final_heading = fmod((initial_heading + rotate_amount), 360.0);

  while (1)
  {
    float current_heading = magnetometer_heading();

    // printf("staring angle: %f | ending angle: %f | current angle: %f\n", initial_heading, final_heading, current_heading);

    // vTaskDelay(pdMS_TO_TICKS(100));


    if (current_heading > final_heading - 10 && current_heading < final_heading + 10)
    {
      // Angle has reached or exceeded 90 degrees, stop the car
      // printf("Reached 90 degrees, stopping the car\n");
      stop();
      vTaskDelay(pdMS_TO_TICKS(100));
      // moveForward();
      // gas();
      break; // Exit the loop and end the task
    }
    else
    {
      turnRight();
      gas();
    }

    turnRight();
    gas();
  }

}




void motor_task(__unused void *params)
{
  // while(1)
  // {
  //     moveForward();
  //     gas();
  // }

  // float startingMag = -1;
  // float angle = 0;
  // float diffAngle = 0;

  // do {
  //   startingMag = magnetometer_heading();
  // } while (startingMag < 0);

  // while (1)
  // {
  //   angle = magnetometer_heading();

  //   diffAngle = angle - startingMag;
  //   if (diffAngle < 0) diffAngle += 360;
  //   printf("staring angle: %f | ending angle: %f | diff angle: %f\n", startingMag, angle, diffAngle);

  //   vTaskDelay(pdMS_TO_TICKS(100));

  //   if (diffAngle >= 80 && diffAngle <= 350)
  //   {
  //     // Angle has reached or exceeded 90 degrees, stop the car
  //     // printf("Reached 90 degrees, stopping the car\n");
  //     stop();
  //     vTaskDelay(pdMS_TO_TICKS(100));
  //     moveForward();
  //     gas();
  //     break; // Exit the loop and end the task
  //   }

  //   turnRight();
  //   gas();
  // }


  // while (1)
  // {
  //     lsm303dlh_mag_setup();
  //     lsm303dlh_read_mag(&mag);
  //     angle = get_angle(&mag);
  //     diffAngle = startingMag - angle ;

  //     if (diffAngle < -15)
  //     {
  //         diffAngle += 360;
  //     }

  //     printf("staring angle: %0.6f | ending angle: %0.6f | diff angle: %0.6f\n", startingMag, angle, diffAngle);
  //     sleep_ms(50);



  //     if (diffAngle >= 85 && diffAngle <= 95)
  //     {
  //         // Angle has reached or exceeded 90 degrees, stop the car
  //         stop();
  //         printf("Reached 90 degrees, stopping the car\n");
  //         break; // Exit the loop and end the task
  //     }

  //     turnLeft();
  //     gas();
  // }

  // moveForward();
  // gas();
}

void wheelencoder_task(__unused void *params)
{
  while(1)
  {
    // int pinState1 = getResults1();
    // getSpeed(pinState1);

    // int pinState2 = getResults2();
    // getSpeed2(pinState2);
  }
}

void ultrasonic_task(__unused void *params)
{
  while(1)
  {
    uint64_t pulseLength = get_measurement_cm();
    // printf("this is distance: %lldcm \n", pulseLength);

    if(pulseLength < 10)
    {
      stop();
    }
    else
    {
      moveForward();
    }
  }
}

void barcode_task(__unused void *params)
{
  while(1)
  {
    adc_callback();
    sleep_ms(100);
  }
}

void magnetometer_task(__unused void *params)
{
   // read
   while (true) {
    // lsm303dlh_acc_setup();
    // printf("Acc. X = %5d Y = %5d, Z = %5d \t Mag. X = %4d Y = %4d, Z = %4d \t Angle = %f \r\n",
    //          acc.x,acc.y,acc.z,mag.x,mag.y,mag.z,angle);
    //   printf("Mag. X = %4d Y = %4d, Z = %4d \t Angle = %f \r\n",
    //            mag.x,mag.y,mag.z,angle);
    //   sleep_ms(REFRESH_PERIOD);
   }
}

void linereader_task(__unused void *params)
{

  float initial_heading = magnetometer_heading();

  while(1)
  {
    adc_callback_lr();

    int checkWall = scan_walls();
    if (checkWall == 3)
    {
      moveForward();
      gas();
      vTaskDelay(pdMS_TO_TICKS(625));
      continue;
    }
    else if (checkWall == 1)
    {
      printf("turning right");
      moveForward();
      gas();
      vTaskDelay(pdMS_TO_TICKS(600));
      rotate_right(90, initial_heading);
      continue;
    }
    else if  (checkWall == 2)
    {
      printf("turning left");
      moveForward();
      gas();
      vTaskDelay(pdMS_TO_TICKS(600));
      rotate_left(90, initial_heading);
      continue;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


void vLaunch( void) {
  // TaskHandle_t motorTask;
  // xTaskCreate(motor_task, "MotorThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &motorTask);

  // TaskHandle_t wheelencodertask;
  // xTaskCreate(wheelencoder_task, "WheelEncoderThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &wheelencodertask);

  TaskHandle_t ultrasonictask;
  xTaskCreate(ultrasonic_task, "UltraSonicThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY + 1, &ultrasonictask);

  // TaskHandle_t barcodetask;
  // xTaskCreate(barcode_task, "BarCodeThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY , &barcodetask);

  TaskHandle_t linereadertask;
  xTaskCreate(linereader_task, "LineReaderThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY , &linereadertask);

  // TaskHandle_t magnetometertask;
  // xTaskCreate(magnetometer_task, "MagnetometerThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &magnetometertask);

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
  motor_init();
  sensor_init();
  ultrasonic_init();
  barcode_init();
  ir_sensor_init();
  magnetometer_init();
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
