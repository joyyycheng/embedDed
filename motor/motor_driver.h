
void motor_init();
void motorControlA(int enable, int in1, int in2);
void motorControlB(int enable, int in3, int in4);
void stop();
void moveBackward();
void moveForward();
void turnRight();
void turnLeft();

void set_motor_left(float dutyCycle);
void set_motor_right(float dutyCycle);


float get_dutyCycleLeft();

float get_dutyCycleRight();
