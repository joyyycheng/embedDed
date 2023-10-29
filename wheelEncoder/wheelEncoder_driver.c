#include <stdio.h> 
#include "pico/stdlib.h" 
#include "hardware/pwm.h" 
 
#include <time.h> 
#include <sys/time.h> 
#include "pico/time.h" 


#define SENSOR1_OUT 6 
#define SENSOR1_VCC 7 
#define SENSOR1_GND 8 

#define SENSOR2_OUT 9 
#define SENSOR2_VCC 10 
#define SENSOR2_GND 11

int counter = 0; 
int wheelCount = 0; 
int previousPinState = 0; 
double speed = 0.0;

static absolute_time_t startTime;
static absolute_time_t endTime;

void sensorInit() 
{ 
    gpio_init(SENSOR1_OUT); 
    gpio_init(SENSOR1_VCC); 
    gpio_init(SENSOR1_GND); 
 
 
    gpio_set_dir(SENSOR1_OUT, GPIO_OUT); 
    gpio_set_dir(SENSOR1_VCC, GPIO_OUT); 
    gpio_set_dir(SENSOR1_GND, GPIO_OUT); 
 
    // Example: Turn ON the OUT pin and VCC, and turn OFF GND 
    gpio_put(SENSOR1_OUT, 1);  // Set OUT pin to HIGH 
    gpio_put(SENSOR1_VCC, 1);  // Set VCC pin to HIGH 
    gpio_put(SENSOR1_GND, 0);  // Set GND pin to LOW 


    gpio_init(SENSOR2_OUT); 
    gpio_init(SENSOR2_VCC); 
    gpio_init(SENSOR2_GND); 
 
 
    gpio_set_dir(SENSOR2_OUT, GPIO_OUT); 
    gpio_set_dir(SENSOR2_VCC, GPIO_OUT); 
    gpio_set_dir(SENSOR2_GND, GPIO_OUT); 
 
    // Example: Turn ON the OUT pin and VCC, and turn OFF GND 
    gpio_put(SENSOR2_OUT, 1);  // Set OUT pin to HIGH 
    gpio_put(SENSOR2_VCC, 1);  // Set VCC pin to HIGH 
    gpio_put(SENSOR2_GND, 0);  // Set GND pin to LOW 
 
} 

bool getResults1 () 
{ 
    int getState = gpio_get(SENSOR1_OUT); 
    //printf("pin state: %d\n", getState); 
 
    return getState; 
} 
 
bool getResults2 (struct repeating_timer t) 
{ 
    int getState = gpio_get(SENSOR2_OUT); 
    //printf("pin state: %d\n", getState); 
 
    return getState; 
} 

void getSpeed(int pinState) {

    if (pinState == 1 && previousPinState == 0) { 
        if (counter == 0) {
            startTime = get_absolute_time();
        } 
        counter++; 
        if(counter % 20 == 0) { 
            endTime = get_absolute_time();
            counter = 0;
            wheelCount++;                

            uint64_t duration_us = absolute_time_diff_us(startTime, endTime);
            double duration_sec = (double)duration_us / 1000000.0;

            // Print or use the time duration as needed  
            printf("Time duration: %.6f seconds\n", duration_sec);

            speed = 19.792 / duration_sec;

            printf("The speed is %.6f cm/s\n", speed);
        }        
    }
    previousPinState = pinState;
}