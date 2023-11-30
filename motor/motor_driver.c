#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include <time.h>
#include <sys/time.h>
#include "pico/time.h"

// Define GPIO pins for motor control
#define MOTOR_ENA_PIN 0 // GPIO0 (Motor A)
#define MOTOR_IN1_PIN 1 // GPIO1
#define MOTOR_IN2_PIN 2 // GPIO2

#define MOTOR_ENB_PIN 3  // GPIO3 (Motor B)
#define MOTOR_IN3_PIN 17 // GPIO4
#define MOTOR_IN4_PIN 16 // GPIO5

// Define initial duty cycles for both motors
volatile float duty_cycle_Left = 12500 / 1.7; // GPIO5
volatile float duty_cycle_Right = 12500 / 1.4;

void trigger_motor_Left()
{
    /**
     * @brief Triggers motors using PWM to turn left
     */

    uint slice_num_motor1 = pwm_gpio_to_slice_num(MOTOR_ENA_PIN); // left

    // Set the PWM frequency and range
    pwm_set_clkdiv(slice_num_motor1, 100); // Divide the default clock frequency by 4
    pwm_set_wrap(slice_num_motor1, 12500);

    // Set PWM duty cycle and enable
    pwm_set_chan_level(slice_num_motor1, PWM_CHAN_A, duty_cycle_Left);
    pwm_set_enabled(slice_num_motor1, true);
}

void trigger_motor_Right()
{
    /**
     * @brief Triggers motors using PWM to turn right
     */

    uint slice_num_motor2 = pwm_gpio_to_slice_num(MOTOR_ENB_PIN);

    // Set the PWM frequency and range
    pwm_set_clkdiv(slice_num_motor2, 100); // Divide the default clock frequency by 4
    pwm_set_wrap(slice_num_motor2, 12500);

    // Set PWM duty cycle and enable
    pwm_set_chan_level(slice_num_motor2, PWM_CHAN_B, duty_cycle_Right);
    pwm_set_enabled(slice_num_motor2, true);
}

void set_motor_left(float dutyCycle)
{
    /**
     * @brief Set and update motor's duty cycle
     */

    duty_cycle_Left = dutyCycle;
    trigger_motor_Left();
}

void set_motor_right(float dutyCycle)
{
    duty_cycle_Right = dutyCycle;
    trigger_motor_Right();
}

// Initialize motor control pins and settings
void motor_init()
{
    /**
     * @brief Initialize motor control pins and settings
     */

    // Initialize GPIOs for motor control
    gpio_init(MOTOR_ENA_PIN);
    gpio_init(MOTOR_IN1_PIN);
    gpio_init(MOTOR_IN2_PIN);

    gpio_set_dir(MOTOR_ENA_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_IN1_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_IN2_PIN, GPIO_OUT);

    gpio_init(MOTOR_ENB_PIN);
    gpio_init(MOTOR_IN3_PIN);
    gpio_init(MOTOR_IN4_PIN);

    gpio_set_dir(MOTOR_ENB_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_IN3_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_IN4_PIN, GPIO_OUT);

    // Set GPIO functions to PWM for motor speed control
    gpio_set_function(MOTOR_ENA_PIN, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_ENB_PIN, GPIO_FUNC_PWM);

    trigger_motor_Left();
    trigger_motor_Right();
}

// Functions to control both motors
void motorControlA(int enable, int in1, int in2)
{
    gpio_put(MOTOR_ENA_PIN, enable);
    gpio_put(MOTOR_IN1_PIN, in1);
    gpio_put(MOTOR_IN2_PIN, in2);
}

void motorControlB(int enable, int in3, int in4)
{
    gpio_put(MOTOR_ENB_PIN, enable);
    gpio_put(MOTOR_IN3_PIN, in3);
    gpio_put(MOTOR_IN4_PIN, in4);
}

void stop()
{
    /**
     * @brief To stop both motors
     */
    motorControlA(0, 1, 1);
    motorControlB(0, 1, 1);
}

void moveBackward()
{
    /**
     * @brief Move both motors backwards
     */
    motorControlA(1, 0, 1);
    motorControlB(1, 0, 1);
}

// To move both motors forward
void moveForward()
{
    motorControlA(1, 1, 0);
    motorControlB(1, 1, 0);
    // sleep_ms(500);
    // stop();
}

void turnRight()
{
    /**
     * @brief Turn motor right
     */
    motorControlA(0, 1, 0);
    motorControlB(1, 0, 1);
}

void turnLeft()
{
    /**
     * @brief Turn motor right
     */

    motorControlA(1, 0, 1);
    motorControlB(0, 1, 0);
}

float get_dutyCycleLeft()
{
    /**
     * @brief Get current duty cycle of left motor
     */
    return duty_cycle_Left;
}

float get_dutyCycleRight()
{
    /**
     * @brief Get current duty cycle of right motor
     */
    return duty_cycle_Right;
}
