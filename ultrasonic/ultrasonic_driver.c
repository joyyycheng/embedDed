#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TIMEOUT 26100
#define TRIGGER_PULSE_US 100

#define gpio_vcc 15
#define gpio_triggerPin 12
#define gpio_echoPin 13

static volatile uint64_t start_time;
static volatile uint64_t end_time;
static volatile bool measurement_fired = false;

void trigger_pulse()
{
    /**
     * @brief Triggers an ultrasonic pulse
     *
     * This function sets the trigger pin to HIGH, waits for a short duration
     * and then sets the trigger pin back to LOW
     */

    gpio_put(gpio_triggerPin, 1);
    sleep_ms(TRIGGER_PULSE_US);
    gpio_put(gpio_triggerPin, 0);
}

void echo_irq_callback(uint gpio, uint32_t events)
{
    /**
     * @brief Handles the rising or falling edge of the echo signal
     *
     * Function is triggered by an interrupt on the echo pin.
     *
     * Manages the measurement process, recording the start and end time
     * based on the change of state in the echo pin
     *
     * Also implementing a timeout condition to prevent forever measurement
     */

    static int timeout_counter = 0;

    if (!measurement_fired && gpio_get(gpio_echoPin))
    {
        /*
        If no measurement is in progress and the echo pin is HIGH,
        then toggle the measurement flag and record the start time,
        and increment the timeout_counter
        */

        measurement_fired = true;
        start_time = time_us_64();
        timeout_counter++;
    }
    else if (measurement_fired && gpio_get(gpio_echoPin))
    {
        /*
        If measurement is in progress and the echo pin is HIGH,
        increment the timeout_counter and proceed to check if it
        meets the timeout condition, if yes then stop the measurement,
        reset the timeout_counter and record the end time
        */

        timeout_counter++;

        if (timeout_counter > TIMEOUT)
        {
            timeout_counter = 0;
            measurement_fired = false;
            end_time = time_us_64();
        }
    }
    else if (measurement_fired && !gpio_get(gpio_echoPin))
    {
        /*
        If measurement is in progress and the echo pin is LOW,
        then set reset the measurement flag, timeout_counter and
        record the end time.
        */

        timeout_counter = 0;
        measurement_fired = false;
        end_time = time_us_64();
    }
}

void ultrasonic_init()
{
    /**
     * @brief Initialize the ultrasonic sensor module
     *
     * Function sets up the GPIO pins for trigger and echo, configures
     * and enables interrupts for the echo pin
     */


    gpio_init(gpio_triggerPin);
    gpio_init(gpio_echoPin);

    gpio_set_dir(gpio_triggerPin, GPIO_OUT);
    gpio_set_dir(gpio_echoPin, GPIO_IN);

    gpio_init(gpio_vcc); 
    // gpio_init(GND_PIN);

    gpio_set_dir(gpio_vcc, GPIO_OUT); 
    // gpio_set_dir(GND_PIN, GPIO_OUT); 

    gpio_put(gpio_vcc, 1);  // Set VCC pin to HIGH 

    gpio_set_irq_enabled_with_callback(gpio_echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_irq_callback);
}

uint64_t get_measurement_cm()
{
    /**
     * @brief Gets the distance measurement in centimeters
     *
     * Function triggers an ultrasonic pulse, measure the pulse length
     * and calculates the distance in centimeters
     */

    trigger_pulse();

    uint64_t pulseLength = end_time - start_time;

    return (pulseLength / 29 / 2);
}