#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "pico/time.h"

#define PWM_PIN 14 // original was 2
#define ADC_PIN 28

#define LIGHT_THRESHOLD 1500  // Adjust this threshold for light/dark line detection. Ideally value between 300-500
#define DARK_THRESHOLD 1800  // Adjust this threshold for wide/narrow line detection. Idealy value between 1800-2400


struct Code39Character {
    char character;
    char pattern[10];
};

struct Code39Character code39_binary[] = {
    {'A', "100001001"},
    {'B', "001001001"},
    {'C', "101001000"},
    {'D', "000011001"},
    {'E', "100011000"},
    {'F', "001011000"},
    {'G', "000001101"},
    {'H', "100001100"},
    {'I', "001001100"},
    {'J', "000011100"},
    {'K', "100000011"},
    {'L', "001000011"},
    {'M', "101000010"},
    {'N', "000010011"},
    {'O', "100010010"},
    {'P', "001010010"},
    {'Q', "000000111"},
    {'R', "100000110"},
    {'S', "001000110"},
    {'T', "000010110"},
    {'U', "110000001"},
    {'V', "011000001"},
    {'W', "111000000"},
    {'X', "010010001"},
    {'Y', "110010000"},
    {'Z', "011010000"},
    {'*', "010010100"},
    {'0', "000110100"},
    {'1', "100100001"},
    {'2', "010100001"},
    {'3', "110100000"},
    {'4', "001100001"},
    {'5', "101100000"},
    {'6', "011100000"},
    {'7', "000100101"},
    {'8', "100100100"},
    {'9', "010100100"},
    {'-', "010001010"},
    {'.', "110001000"},
    {' ', "011001000"},
    {'$', "010110000"},
    {'/', "010101000"},
    {'+', "010001010"},
    {'%', "000010010"}
};

// Function to decode the binary to respective letter or symbol
char decode_code39(const char* binary_pattern, struct Code39Character* dictionary) {
    for (int i = 0; i < 44; i++) {
        if (strcmp(binary_pattern, dictionary[i].pattern) == 0) {
            return dictionary[i].character;
        }
    }
    return '\0';  // Pattern not found in the dictionary.
}

// Function to get the current time in "hh:mm:ss:ms" format
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

void printBinary(int num) {
    if (num) {
        printBinary(num >> 1);
        putchar((num & 1) ? '1' : '0');
    }
}

char* binaryToChar(int num) {
    static char binaryStr[10];  // Allocate a char array to hold binary representation
    binaryStr[9] = '\0';       // Null-terminate the string

    for (int i = 8; i >= 0; i--) {
        binaryStr[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }

    return binaryStr;
}

bool adc_callback()
{
    static bool on_dark_line = false;  // Flag to track whether the sensor is on a dark line
    static bool first_dark_line_detected = false; // Flag to track the first dark line detection

    static uint32_t dark_line_duration = 0;  // Duration on a dark line
    static uint32_t white_line_duration = 0;  // Duration on a light line

    static uint32_t black_line_count = 0;
    static uint32_t white_line_count = 0;
    static uint32_t total_line_count = 0;

    static uint32_t wide_threshold = 0;

    static uint32_t line_array[9];

    // Initialize a string to store the binary representation
    static int bit_array = 0;
    char *current_time = get_time();
    int32_t adc_reading = adc_read();

    //printf("%s -> ADC Value: %d\n", current_time, adc_reading);

    static uint8_t data_buffer = 0;  // Buffer to store binary data
    static uint8_t bit_count = 0;   // Count of bits in the buffer
    // Implement your barcode detection logic here
    if (adc_reading < LIGHT_THRESHOLD) { //If ADC Value is below 500, it is on white line
        // On a white line
        //printf("%s -> On White Line\n", current_time);
        //printf("%s -> White Line Duration: %d\n", current_time, white_line_duration);
        if (on_dark_line) { //Initially on dark line
            // Transition from dark to light
            if (first_dark_line_detected && black_line_count <= 4) {
                printf("Dark %d count: %d\n", black_line_count, dark_line_duration); //Black 0 count: 40
                line_array[total_line_count] = dark_line_duration;
                printf("ARRAY %d: %d\n", total_line_count ,line_array[total_line_count]);
                total_line_count++;
            }
            if (white_line_count >= 4){
                //Do calculation here
                int largest_value = line_array[0];
                for (int i = 0; i < 9; i++) { // Loop through the array to find the biggest value
                    //printf("ARRAY %d: %d\n", i, line_array[i]);
                    if (line_array[i] > largest_value) {
                        largest_value = line_array[i];
                    }
                }
                //Algorithm to determine narrow or wide
                wide_threshold = largest_value / 2;
                
                for (int i = 0; i < 9; i++) {
                    if (line_array[i] > wide_threshold) {
                        // If the value is wide, set the corresponding bit to 1
                        bit_array |= (1 << (8 - i));
                    } else {
                        // If the value is not wide, set the corresponding bit to 0
                        bit_array &= ~(1 << (8 - i));
                    }
                }

                //const char* barcode_data = bit_array
                //printf("Bit Array: %d (binary: ", bit_array);
                printf("Binary Scan Result: ");
                printBinary(bit_array);
                printf("\n");

                // Decode the barcode data
                char* binary_pattern = binaryToChar(bit_array); // Replace with the actual binary pattern you want to decode
                printf("Binary Pattern: %s\n", binary_pattern);
                char decoded_character = decode_code39(binary_pattern, code39_binary);
                //printf("decoded character: %s\n", decoded_character);
                
                if (decoded_character != '\0') {
                    printf("Decoded Character: %c\n", decoded_character);
                } else {
                    printf("Character not found.\n");
                }

                black_line_count = 0;
                white_line_count = 0;
                on_dark_line = false;
                first_dark_line_detected = false;
                total_line_count = 0;
            }
            on_dark_line = false; //Now on white line
            dark_line_duration = 0;
            // Interpret duration to determine bit (0 for narrow, 1 for wide)
            uint8_t bit_value = (white_line_duration > DARK_THRESHOLD) ? 1 : 0;
            data_buffer = (data_buffer << 1) | bit_value;
            bit_count++;

            // If you've collected 8 bits, process the character
            if (bit_count == 8) {
                // Process the character (e.g., display it, decode it)
                printf("Character: %c\n", data_buffer);
            // Reset the buffer and bit count for the next character
                data_buffer = 0;
                bit_count = 0;
            }
        }
        white_line_duration++;
    } else if (adc_reading > DARK_THRESHOLD) {
        //printf("%s -> On Dark Line\n", current_time);
        //printf("%s -> Dark Line Duration: %d\n", current_time, dark_line_duration);
        // On a dark line
        if (!on_dark_line) { //If change from white to black line
            // Transition from light to dark
            if (first_dark_line_detected) {
                printf("White %d count: %d\n", white_line_count, white_line_duration); //Black 0 count: 40
                line_array[total_line_count] = white_line_duration;
                printf("ARRAY %d: %d\n", total_line_count, line_array[total_line_count]);
                black_line_count++;
                white_line_count++;
                total_line_count++;
            }
            on_dark_line = true;
            white_line_duration = 0;
        }
        if (!first_dark_line_detected) { // If it is the first time it changed, barcode start detection
            first_dark_line_detected = true; // Flag to track the first dark line detection
        }
        dark_line_duration++;
    }

    return true;
}

void barcodeInit()
{
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_clkdiv(slice_num, 100);                  // Adjust the clock divider, PWM signal frequency reduced by factor of 100
    pwm_set_wrap(slice_num, 62500);                  // Period of the PWM signal
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1250); // 50% duty cycle, high for half, low for other half
    pwm_set_enabled(slice_num, true);

    // Initialize ADC on GP26 (ADC_PIN)
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0); // Use ADC channel 0 as GP26 is ADC0
}

