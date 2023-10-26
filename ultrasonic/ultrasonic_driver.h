#include <stdint.h>

void trigger_pulse_high(struct repeating_timer t);
void trigger_pulse_low(struct repeating_timer t );
void trigger_pulse_callback(struct repeating_timer *t);
void ultrasonic_init();
uint64_t get_measurement_cm();
