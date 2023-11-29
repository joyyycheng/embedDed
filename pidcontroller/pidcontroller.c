#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include <time.h> 
#include <sys/time.h> 
#include "pico/time.h" 
#include "motor_driver.h"
#include "wheelEncoder_driver.h"

// Define GPIO pins for motor control
#define MOTOR_ENA_PIN 0 // GPIO0 (Motor A)
#define MOTOR_IN1_PIN 1 // GPIO1 
#define MOTOR_IN2_PIN 2 // GPIO2 
 
#define MOTOR_ENB_PIN 3 // GPIO3 (Motor B)
#define MOTOR_IN3_PIN 17 // GPIO4 
#define MOTOR_IN4_PIN 16 // GPIO5 

#define MAX_DUTY 12500 // Maximum duty cycle value


// PID control function for Motor A
float compute_pid1(float setpoint, float current_value, float *integral, float *prev_error)
{
    // PID controller parameters
    float Kp = 0.2; // Proportional gain
    float Ki = 0.1; // Integral gain
    float Kd = 0.00; // Derivative gain

    // PID error calculations
    float error = current_value - setpoint;
    *integral += error;
    float derivative = error - *prev_error;
    float control_signal = Kp * error + Ki * (*integral) + Kd * derivative * 0.1; //diff from jason
    *prev_error = current_value;
    return control_signal;
}

// PID control function for Motor B
float compute_pid2(float setpoint, float current_value, float *integral, float *prev_error)
{
    // PID controller parameters
    float Kp = 0.6; // Proportional gain
    float Ki = 0.3; // Integral gain
    float Kd = 0.00; // Derivative gain

    // PID error calculations
    float error = current_value - setpoint;
    *integral += error;
    float derivative = error - *prev_error;
    float control_signal = Kp * error + Ki * (*integral) + Kd * derivative * 0.1; //diff from jason
    *prev_error = current_value;
    return control_signal;
}


static char msg[100]; // Message buffer for debugging

// PID control variables for Motor A and B
float integral1 = -1;                 // accumulating past errors (CAN CHANGE IF U WANT)
float prev_error1 = 0;               // store value of error from previous iteration of loop

float integral2 = 2;                 // accumulating past errors (CAN CHANGE IF U WANT)
float prev_error2 = 0;               // store value of error from previous iteration of loop

// Function to accelerate forward
void *gas()
{
    // initialise variables
    float setpoint = 33;             // target speed for the motors (TO BE CHANGED)


    // Get current speed of Motor B and A
    int pinState2 = getResults2();
    float current_speed_B = getSpeed2(pinState2); // measure speed of motor B
    int pinState1 = getResults1();
    float current_speed_A = getSpeed(pinState1); // measure speed of motor A

    // Calculate control signal for Motor A and B
    float control_signal_A = compute_pid1(setpoint, current_speed_A, &integral1, &prev_error1);
    float control_signal_B = compute_pid2(setpoint, current_speed_B, &integral2, &prev_error2);

    // Calculate new duty cycles based on control signals
    float duty_cycle_A = get_dutyCycleLeft() * control_signal_A + get_dutyCycleLeft();
    float duty_cycle_B = get_dutyCycleRight() * control_signal_B + get_dutyCycleRight();

    // Set new duty cycles to the motors
    set_motor_left(duty_cycle_A);
    set_motor_right(duty_cycle_B);
}
