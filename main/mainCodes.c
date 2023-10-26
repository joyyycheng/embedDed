#include <stdio.h> 
#include "pico/stdlib.h" 
#include "hardware/pwm.h" 
 
#include <time.h> 
#include <sys/time.h> 
#include "pico/time.h" 

#include "motor_driver.h"
#include "wheelEncoder_driver.h"
#include "ultrasonic_driver.h"

 
int main() { 
    
    stdio_init_all(); 
    motorInit(); 
    sensorInit(); 
    ultrasonic_init();

    struct repeating_timer timer1; 
    //struct repeating_timer timer2;
    struct repeating_timer sonicTimer;

    add_repeating_timer_ms(-25, (repeating_timer_callback_t)getResults1, NULL, &timer1); 
    // add_repeating_timer_ms(-25, (repeating_timer_callback_t)getResults2, NULL, &timer2); 
    add_repeating_timer_ms(-500, (repeating_timer_callback_t)trigger_pulse_callback, NULL, &sonicTimer); 
    
    


 
 
    while (1) 
    { 

        moveForward(); 
        tight_loop_contents();
        int pinState1 = getResults1(NULL); 
        //int pinState2 = getResults2(NULL);
        getSpeed(pinState1);
        //getSpeed(previousPinState2, counter2, wheelCount2, startTime2, endTime2, &speed2, pinState2);

        uint64_t pulseLength = get_measurement_cm();
        printf("this is distance: %lldcm \n", pulseLength);
        
    }   
    return 0;
}