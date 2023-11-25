float compute_pid1(float setpoint, float current_value, float *integral, float *prev_error);
float compute_pid2(float setpoint, float current_value, float *integral, float *prev_error);
void *gas();