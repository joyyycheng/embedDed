/*
  GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (e.g. pin 6)) -> SDA
  GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (e.g. pin 7)) -> SCL
  3.3v (e.g. pin 36) -> VCC
  GND (e.g. pin 38)  -> GND
*/

// Required Dependencies
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "magnetometer_driver.h"

// Parameter Constants
#define I2C_BAUD 100       // 100 kHz for Baud Rate
#define REFRESH_PERIOD 100 // 100ms
#define X_OFFSET -27.5 // Hard-iron Calibration: X offset
#define Y_OFFSET  20.0 // Hard-iron Calibration: Y offset
#define MAG_DECL  0.05 //  Magnetic Declination: https://www.magnetic-declination.com/SINGAPORE/SINGAPORE/2443440.html

// Function Prototypes
void init_i2c_default();

// Setup Function
void lsm303dlh_mag_setup()
{
  sleep_ms(1000);
  uint8_t buffer[2];
  buffer[0] = MAG_CRA; buffer[1] = 0x18; // CRA_REG_M: 75Hz(0x18) Data Output Rate
  i2c_write_blocking( i2c_default, MAG_SLV, buffer, 2, true );
  buffer[0] = MAG_CRB; buffer[1] = 0xE0; // CRB_REG_M: Gain X,Y,Z at 230 LSB/Gauss
  i2c_write_blocking( i2c_default, MAG_SLV, buffer, 2, true );
  buffer[0] = MAG_MRR; buffer[1] = 0x00; //  MR_REG_M: Continuous-Conversion Mode
  i2c_write_blocking( i2c_default, MAG_SLV, buffer, 2, false );
}

// Read Function (and Hard-iron Calibration for X, Y)
void lsm303dlh_read_mag(mag_t *mag)
{
  uint8_t buffer[6];
  int16_t magnet[3];
  uint8_t reg = MAG_OUT;
  i2c_write_blocking( i2c_default, MAG_SLV, &reg, 1, true );
  i2c_read_blocking( i2c_default, MAG_SLV, buffer, 6, false );
  for (int i = 0; i < 3; i++) {
    // Retrieve Buffer and Insert to magnet[i] based on for-loop i
    magnet[i] = ((buffer[i * 2] << 8 | buffer[(i * 2) + 1] ));
  }
  mag->x = magnet[0] + X_OFFSET; // OUT_X_M: 0x03 & 0x04
  mag->y = magnet[2] + Y_OFFSET; // OUT_Y_M: 0x07 & 0x08
  mag->z = magnet[1];            // OUT_Z_M: 0x05 & 0x06
}

// Calculate Angle Function
float lsm303dlh_get_angle(mag_t *mag)
{
  // Set for Inverse Z-axis, where cables pointing down
  // Correct for Magnetic Declination
  // Convert Heading from Radian to Degrees
  float heading = ( 1 - atan2f( (float)mag->y, (float)mag->x ) ) * ( 180.0 / M_PI ) + MAG_DECL;

  // Correct for Degrees Range
  if (heading < 0.0) heading += 360.0;
  if (heading > 360.0) heading -= 360.0;

  return heading;
}

// Initialise Sensor Function
void magnetometer_init()
{
  init_i2c_default(); // Call I2C Defaults
  lsm303dlh_mag_setup(); // Call Magnetometer Setup
}

// Output Reading Function
float magnetometer_heading()
{
  mag_t mag; // Initialise Magnetometer Structure
  lsm303dlh_read_mag(&mag); // Read Magnetometer with Structure
  return lsm303dlh_get_angle(&mag); // Return Magnetometer Readings as Angles
}

// Main Function for Testing
// int main() {
//   mag_t mag;
//   stdio_init_all();
//   magnetometer_init();

//   while (true) {
//     lsm303dlh_read_mag(&mag);
//     float angle = lsm303dlh_get_angle(&mag);
//     printf("%4d, %4d, %f\r\n", mag.x,mag.y,angle);
//     sleep_ms(REFRESH_PERIOD);
//   }
//   return 0;
// }

// Initialise I2C Defaults
void init_i2c_default()
{
   i2c_init(i2c_default, I2C_BAUD * 1000); // Initialise I2C and set Baud Rate
   gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); // Set Serial Data Pin for I2C
   gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C); // Set Serial Clock Pin for I2C
   gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN); // Set GPIO Pull Up for SDA Pin
   gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN); // Set GPIO Pull Up for SCL Pin
}
