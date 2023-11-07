#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include <time.h> 
#include <sys/time.h> 
#include "pico/time.h" 
#include "motor_driver.h"
#include "wheelEncoder_driver.h"

#define MOTOR_ENA_PIN 0 // GPIO0 
#define MOTOR_IN1_PIN 1 // GPIO1 
#define MOTOR_IN2_PIN 2 // GPIO2 
 
#define MOTOR_ENB_PIN 3 // GPIO3 
#define MOTOR_IN3_PIN 17 // GPIO4 
#define MOTOR_IN4_PIN 16 // GPIO5 

#define MAX_DUTY 12500

float compute_pid1(float setpoint, float current_value, float *integral, float *prev_error)
{
    // initializing Kp, Ki, Kd values
    float Kp = 0.5;
    float Ki = 0.3;
    float Kd = 0.00;

    float error = current_value - setpoint;
    *integral += error;
    float derivative = error - *prev_error;
    float control_signal = Kp * error + Ki * (*integral) + Kd * derivative * 0.1; //diff from jason
    *prev_error = current_value;
    return control_signal;
}

float compute_pid2(float setpoint, float current_value, float *integral, float *prev_error)
{
    // initializing Kp, Ki, Kd values
    float Kp = 0.3;
    float Ki = 0.1;
    float Kd = 0.00;

    float error = current_value - setpoint;
    *integral += error;
    float derivative = error - *prev_error;
    printf("error: %f, integral: %f, derivative %f\n", error, (*integral), derivative);
    float control_signal = Kp * error + Ki * (*integral) + Kd * derivative * 0.1; //diff from jason
    *prev_error = current_value;
    return control_signal;
}


static char msg[100];

float integral1 = 1;                 // accumulating past errors (CAN CHANGE IF U WANT)
float prev_error1 = 0;               // store value of error from previous iteration of loop

float integral2 = 0;                 // accumulating past errors (CAN CHANGE IF U WANT)
float prev_error2 = 0;               // store value of error from previous iteration of loop

// accelerate forward
void *gas()
{
    // initialise variables
    float setpoint = 32;             // target speed for the motors (TO BE CHANGED)
    int pinState1 = getResults1();
    float current_speed_A = getSpeed(pinState1); // measure speed of motor A

    int pinState2 = getResults2();
    float current_speed_B = getSpeed2(pinState2); // measure speed of motor B

    // control signal of A
    float control_signal_A = compute_pid1(setpoint, current_speed_A, &integral1, &prev_error1);
    //printf("control signal left: %0.6f | ", control_signal_A);

    // control signal of B
    float control_signal_B = compute_pid2(setpoint, current_speed_B, &integral2, &prev_error2);
    //printf("control signal right: %0.6f\n", control_signal_B);
    // get PWM slice
    // uint slice_num_A = pwm_gpio_to_slice_num(MOTOR_ENA_PIN);
    // uint slice_num_B = pwm_gpio_to_slice_num(MOTOR_ENB_PIN);

    // adjust PWM duty cycle to the control signal
    // 12500 max
    // 10000 default hz
    // current speed 30cm/s, target 32
    // control_ssignal_A = 0.9 of Hz

    // 10000 * 0.9 + 10000 = 10900 Hz

    // set new Hz - 10900 Hz. 2nd speed: 31
    // control_ = 0.2 of Hz
    // 10900 * 0.2 + 10900 = 11500 Hz

    // 3rd speed: 32.5
    // -0.01
    float duty_cycle_A = get_dutyCycleLeft() * control_signal_A + get_dutyCycleLeft();
    float duty_cycle_B = get_dutyCycleRight() * control_signal_B + get_dutyCycleRight();

    set_motor_left(duty_cycle_A);
    set_motor_right(duty_cycle_B);


    // pwm_set_chan_level(slice_num_A, PWM_CHAN_A, duty_cycle_A);
    // pwm_set_chan_level(slice_num_B, PWM_CHAN_B, duty_cycle_B);

    // gpio_put(MOTOR_IN1_PIN, 0);
    // gpio_put(MOTOR_IN2_PIN, 1);

    // gpio_put(MOTOR_IN3_PIN, 0);
    // gpio_put(MOTOR_IN4_PIN, 1);
    //printf(msg, "A: %d - B: %d\n", duty_cycle_A, duty_cycle_B);
    // return msg;


}