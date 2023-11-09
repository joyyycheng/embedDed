#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "pico/time.h"

#define VCC_PIN_LEFT 18 //Red
#define ADC_PIN_LEFT 27

#define VCC_PIN_RIGHT 19 //Brown
#define ADC_PIN_RIGHT 28

#define LIGHT_THRESHOLD 1500  // Adjust this threshold for light/dark line detection. Ideally value between 300-500
#define DARK_THRESHOLD 1800  // Adjust this threshold for wide/narrow line detection. Ideally value between 1800-2400


// Direction constants
#define UP 0b1000       // Int value - 8 
#define RIGHT 0b0100    // Int value - 4
#define DOWN 0b0010     // Int value - 2
#define LEFT 0b0001     // Int value - 1

char *get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    static char time_str[20]; // Static buffer to hold the formatted time

    // Format the time to "hh:mm:ss"
    strftime(time_str, sizeof(time_str), "%H:%M:%S", gmtime(&tv.tv_sec));

    // Append milliseconds to the formatted time
    sprintf(time_str + 8, ":%03ld", tv.tv_usec / 1000);

    return time_str;
}

char* intToBinaryString(int num) {
        static char binaryString[33];
        int i;
        for (i = 3; i >= 0; i--) {
            binaryString[3 - i] = ((num >> i) & 1) ? '1' : '0';
        }
        binaryString[4] = '\0'; // Null-terminate the string
        return binaryString;
    }

void ir_sensor_init()
{
    // Initialize VCC pins for both sensors
    gpio_set_function(VCC_PIN_LEFT, GPIO_FUNC_PWM);
    gpio_set_function(VCC_PIN_RIGHT, GPIO_FUNC_PWM);

    gpio_init(VCC_PIN_LEFT);
    gpio_init(VCC_PIN_RIGHT);

    gpio_set_dir(VCC_PIN_LEFT, GPIO_OUT);
    gpio_set_dir(VCC_PIN_RIGHT, GPIO_OUT);

    gpio_put(VCC_PIN_LEFT, 1);  // Set VCC pin to HIGH for left sensor
    gpio_put(VCC_PIN_RIGHT, 1);  // Set VCC pin to HIGH for right sensor

    // Initialize PWM for the left sensor
    uint slice_num_left = pwm_gpio_to_slice_num(VCC_PIN_LEFT);
    pwm_set_clkdiv(slice_num_left, 100);                  // Adjust the clock divider, PWM signal frequency reduced by a factor of 100
    pwm_set_wrap(slice_num_left, 62500);                  // Period of the PWM signal
    pwm_set_chan_level(slice_num_left, PWM_CHAN_A, 1250); // 50% duty cycle, high for half, low for the other half
    pwm_set_enabled(slice_num_left, true);

    // Initialize PWM for the right sensor
    uint slice_num_right = pwm_gpio_to_slice_num(VCC_PIN_RIGHT);
    pwm_set_clkdiv(slice_num_right, 100);                  // Adjust the clock divider, PWM signal frequency reduced by a factor of 100
    pwm_set_wrap(slice_num_right, 62500);                  // Period of the PWM signal
    pwm_set_chan_level(slice_num_right, PWM_CHAN_A, 1250); // 50% duty cycle, high for half, low for the other half
    pwm_set_enabled(slice_num_right, true);

    // Initialize ADC for both sensors
    adc_init();
    
    // Initialize ADC for the left sensor
    adc_gpio_init(ADC_PIN_LEFT);
    adc_select_input(1);  // Use ADC channel 1 for the left sensor

    // Initialize ADC for the right sensor
    adc_gpio_init(ADC_PIN_RIGHT);
    adc_select_input(2);  // Use ADC channel 2 for the right sensor
    
}

int scan_walls(int carDirection)
{
    adc_select_input(1);  // Use ADC channel 1 for the left sensor
    int32_t adc_scan_left = adc_read(); // Reads from ADC channel 1 (left sensor)

    adc_select_input(2);  // Use ADC channel 2 for the right sensor
    int32_t adc_scan_right = adc_read(); // Reads from ADC channel 2 (right sensor)

    // Initialize the wall variable to 0 (no walls detected)
    int walls = 0;

    char *current_time = get_time();

    printf("%s -> Left ADC Value: %d\n", current_time, adc_scan_left);
    printf("%s -> Right ADC Value: %d\n", current_time, adc_scan_right);

    // If the left sensor detects a wall, add the left wall bit to the wall variable
    if (adc_scan_left > DARK_THRESHOLD) {
        walls |= LEFT;
        printf("Left wall added: %04b\n", walls);  // Print the detected walls in binary format
    }

    // If the right sensor detects a wall, add the right wall bit to the wall variable
    if (adc_scan_right > DARK_THRESHOLD) {
        walls |= RIGHT;
        printf("Right wall added: %04b\n", walls);  // Print the detected walls in binary format
    }

    if (carDirection == RIGHT) //4
    {
        walls = (walls >> 1) | (walls << 3);
        printf("CAR FACE RIGHT\n");
    }
    else if (carDirection == DOWN) //2
    {
        walls = (walls >> 2) | (walls << 2);
        printf("CAR FACE DOWN\n");
    }
    else if (carDirection == LEFT) //1
    {
        walls = (walls >> 3) | (walls << 1);
        printf("CAR FACE LEFT\n");
    }
    // Rotate the wall variable to match the car's direction this doesnt work 
    // walls = (walls >> (4 - carDirection)) | (walls << carDirection);

    // Mask the result to 4 bits
    walls &= 0b1111;

    // Array of direction names
    char *directions[] = {"up", "right", "down", "left"};

    // Iterate through each bit in the walls variable
    for (int i = 0; i < 4; i++) {
        // Check if the i-th bit is set
        if ((walls >> i) & 1) {
            // If the bit is set, print that there is a wall in the corresponding direction
            printf("There is a wall %s.\n", directions[3 - i]);
        } else {
            // If the bit is not set, print that there is no wall in the corresponding direction
            printf("There is no wall %s.\n", directions[3 - i]);
        }
    }

    return walls; //Return where there is a wall on the left or right proportional to the car.
    //In the main code I will have to check twice, once facing left or right, then once facing top or down. 
    //Car has to turn once in each cell. Then I will use OR to combine both values of walls and add it to the mapped maze 2d array.
}

bool adc_callback(struct repeating_timer *t)
{
    //static bool on_dark_line_left = false;  // Flag for left IR Sensor 
    //static bool on_dark_line_right = false;  // Flag for right IR Sensor 

    char *current_time = get_time();

    adc_select_input(1);  // Use ADC channel 1 for the left sensor
    int32_t adc_reading_left = adc_read(); // Reads from ADC channel 1 (left sensor)
    adc_select_input(2);  // Use ADC channel 2 for the right sensor
    int32_t adc_reading_right = adc_read(); // Reads from ADC channel 2 (right sensor)

    printf("%s -> Left ADC Value: %d\n", current_time, adc_reading_left);
    printf("%s -> Right ADC Value: %d\n", current_time, adc_reading_right);
    
    return true;
}

int main()
{
    // Initialize stdio for debugging (optional)
    stdio_init_all();

    ir_sensor_init();

    struct repeating_timer timer;

    //add_repeating_timer_ms(-500, adc_callback, NULL, &timer); //Value determines how often ADC value is printed 

    while (1) { 
        char character = getchar();
        printf("%c", character);
        switch (character) {
            case 'r': {
                // Call the custom reset function
                //reset_function();
                add_repeating_timer_ms(-100, adc_callback, NULL, &timer); //Value determines how often ADC value is printed 
                break;
            }
            case 's': {
                printf("Enter current direction of the car (4 for right, 2 for down, 1 for left, default is up): ");
                int currentDirection = getchar() - '0';  // Convert from ASCII to integer
                printf("\nCurrent direction: %d\n", currentDirection);
                int walls = scan_walls(currentDirection);
                printf("Detected walls: %04b\n", walls);  // Print the detected walls in binary format
                break;
            }
            case 'q': {
                return 0;  // Exit the program
            }
        }
    }
}

