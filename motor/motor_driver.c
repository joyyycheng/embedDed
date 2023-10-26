#include <stdio.h> 
#include "pico/stdlib.h" 
#include "hardware/pwm.h" 
 
#include <time.h> 
#include <sys/time.h> 
#include "pico/time.h" 

#define MOTOR_ENA_PIN 0 // GPIO0 
#define MOTOR_IN1_PIN 1 // GPIO1 
#define MOTOR_IN2_PIN 2 // GPIO2 
 
#define MOTOR_ENB_PIN 3 // GPIO3 
#define MOTOR_IN3_PIN 4 // GPIO4 
#define MOTOR_IN4_PIN 5 // GPIO5 


void motorInit() {  
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

    gpio_set_function(MOTOR_ENA_PIN, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_ENB_PIN, GPIO_FUNC_PWM);
    // Initialize PWM for motor speed control 
    uint slice_num_motor1 = pwm_gpio_to_slice_num(MOTOR_ENA_PIN); 
    uint slice_num_motor2 = pwm_gpio_to_slice_num(MOTOR_ENB_PIN); 
 
    // Set the PWM frequency and range 
    pwm_set_clkdiv(slice_num_motor1, 100); // Divide the default clock frequency by 4 
    pwm_set_clkdiv(slice_num_motor2, 100); // Divide the default clock frequency by 4 
 
    pwm_set_wrap(slice_num_motor1, 12000); 
    pwm_set_wrap(slice_num_motor2, 12500); 
 
    pwm_set_chan_level(slice_num_motor1, PWM_CHAN_A, 12000/1.8); 
    pwm_set_chan_level(slice_num_motor2, PWM_CHAN_B, 12500/1.5);   
 
    pwm_set_enabled(slice_num_motor1, true); 
    pwm_set_enabled(slice_num_motor2, true); 
 
} 


void motorControlA(int enable, int in1, int in2) { 
    gpio_put(MOTOR_ENA_PIN, enable); 
    gpio_put(MOTOR_IN1_PIN, in1); 
    gpio_put(MOTOR_IN2_PIN, in2); 
} 
 
void motorControlB(int enable, int in3, int in4) { 
    gpio_put(MOTOR_ENB_PIN, enable); 
    gpio_put(MOTOR_IN3_PIN, in3); 
    gpio_put(MOTOR_IN4_PIN, in4); 
} 
 
void stop() 
{ 
    motorControlA(0, 1, 1); 
    motorControlB(0, 1, 1); 
} 
 
void moveBackward() 
{ 
    motorControlA(1, 1, 0); 
    motorControlB(1, 0, 1); 
    sleep_ms(500); 
    stop(); 
} 
 
void moveForward() 
{ 
    motorControlA(1, 0, 1); 
    motorControlB(1, 1, 0); 
    // sleep_ms(500); 
    // stop(); 
} 
 
void turnRight(int angle) 
{ 
    motorControlA(1, 0, 1); 
    motorControlB(1, 0, 1); 
    sleep_ms(angle); // if it is 500ms it will turn 180 degrees instead of 90 degress 
    stop(); 
} 
 
void turnLeft(int angle) 
{ 
    motorControlA(1, 1, 0); 
    motorControlB(1, 1, 0); 
    sleep_ms(angle); // if it is 500ms it will turn 180 degrees instead of 90 degrees 
    stop(); 
} 