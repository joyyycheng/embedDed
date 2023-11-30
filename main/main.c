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

#include "message_buffer.h"
#define WHEELENCODER_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif
#define TEST_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

#define mbaTASK_MESSAGE_BUFFER_SIZE (60)

void rotate_left(float rotate_amount, float initial_heading)
{
  /**
   * @brief Motor to rotate left by getting the two desired turning value
   * and the current heading
   */

  float final_heading = fmod((initial_heading - rotate_amount + 360), 360.0);

  while (1)
  {
    float current_heading = magnetometer_heading();

    if (current_heading > final_heading - 10 && current_heading < final_heading + 10)
    {
      printf("current heading: %f\n", current_heading);
      stop();
      vTaskDelay(pdMS_TO_TICKS(100));
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
  /**
   * @brief Motor to rotate right by getting the desired two turning value
   * and the current heading
   */

  float final_heading = fmod((initial_heading + rotate_amount), 360.0);

  while (1)
  {
    float current_heading = magnetometer_heading();

    if (current_heading > final_heading - 10 && current_heading < final_heading + 10)
    {
      stop();
      vTaskDelay(pdMS_TO_TICKS(100));
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
  /**
   * @brief FreeRTOS task to move the car forward
   */

  while (1)
  {
    moveForward();
    gas();
  }
}

void ultrasonic_task(__unused void *params)
{
  /**
   * @brief FreeRTOS task to stop the car upon detecting an object infront
   */

  while (1)
  {
    uint64_t pulseLength = get_measurement_cm();
    printf("this is distance: %lldcm \n", pulseLength);

    if (pulseLength <= 15)
    {
      stop();
      sleep_ms(1000);
      moveBackward();
      gas();
      sleep_ms(600);
    }
    else
    {
      moveForward();
      gas();
    }
  }
}

void barcode_task(__unused void *params)
{
  /**
   * @brief FreeRTOS task to detect the barcode
   */

  while (1)
  {
    adc_callback();
    sleep_ms(1);
  }
}

void linereader_task(__unused void *params)
{
  /**
   * @brief FreeRTOS task to detect the lines
   */

  float initial_heading = magnetometer_heading();

  while (1)
  {
    adc_callback_lr();

    if (scan_walls() == 3)
    {
      moveForward();
      gas();
      sleep_ms(100);
      continue;
    }
    else if (scan_walls() == 1)
    {
      moveForward();
      gas();
      vTaskDelay(pdMS_TO_TICKS(600));
      rotate_right(80, initial_heading);
      continue;
    }
    else if (scan_walls() == 2)
    {
      moveForward();
      gas();
      vTaskDelay(pdMS_TO_TICKS(600));
      rotate_left(70, initial_heading);
      continue;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void vLaunch(void)
{
  /**
   * @brief Main FreeRTOS tasks function
   */

  TaskHandle_t motorTask;
  xTaskCreate(motor_task, "MotorThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &motorTask);

  TaskHandle_t ultrasonictask;
  xTaskCreate(ultrasonic_task, "UltraSonicThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &ultrasonictask);

  TaskHandle_t barcodetask;
  xTaskCreate(barcode_task, "BarCodeThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &barcodetask);

  TaskHandle_t linereadertask;
  xTaskCreate(linereader_task, "LineReaderThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &linereadertask);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
  // we must bind the main task to one core (well at least while the init is called)
  // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
  // takes care of it otherwise)
  vTaskCoreAffinitySet(task, 1);
#endif

  /* Start the tasks and timer running. */
  vTaskStartScheduler();
}

int main(void)
{
  /**
   * @brief main function that initialize all sensors
   */

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
#if (portSUPPORT_SMP == 1)
  rtos_name = "FreeRTOS SMP";
#else
  rtos_name = "FreeRTOS";
#endif

#if (portSUPPORT_SMP == 1) && (configNUM_CORES == 2)
  printf("Starting %s on both cores:\n", rtos_name);
  vLaunch();
#elif (RUN_FREERTOS_ON_CORE == 1)
  printf("Starting %s on core 1:\n", rtos_name);
  multicore_launch_core1(vLaunch);
  while (true)
    ;
#else
  printf("Starting %s on core 0:\n", rtos_name);
  vLaunch();
#endif
  return 0;
}
