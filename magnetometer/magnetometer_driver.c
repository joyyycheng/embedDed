/*
   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (e.g. pin 6)) -> SDA
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (e.g. pin 7)) -> SCL
   3.3v (e.g. pin 36) -> VCC
   GND (e.g. pin 38)  -> GND
*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "magnetometer_driver.h"

#define I2C_BAUD 100 // 400 or 100 (kHz)
#define REFRESH_PERIOD 100 // ms

float xMin = 100000;
float xMax = -100000;
float yMin = 100000;
float yMax = -100000;
float xBias, yBias, xScale;

void init_i2c_default();

void lsm303dlh_mag_setup() {
   sleep_ms(1000);
   uint8_t buffer[2];
   buffer[0] = MAG_CRA; buffer[1] = 0x18; // CRA_REG_M: 75Hz(0x18) 30Hz(0x14) 15Hz(0x10) Data Output Rate
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, true );
   buffer[0] = MAG_CRB; buffer[1] = 0xE0; // CRB_REG_M: Gain X,Y,Z at 230 LSB/Gauss
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, true );
   buffer[0] = MAG_MR;  buffer[1] = 0x00; //  MR_REG_M: Continuous-Conversion Mode
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, false );
}

void lsm303dlh_mag_hard_iron() {
   // sleep_ms(1000);
   // float readings[][2] = {
   //    {53, 40},    // 0 deg
   //    {-7, 44},    // 45
   //    {-50, -26},  // 90
   //    {-11, -80},  // 135
   //    {38, -87},   // 180
   //    {70, -65},   // 225
   //    {85, -40},   // 270
   //    {85, -1},    // 315
   // };
   // for (int i = 0; i < 8; i++)
   // {
   //    if (readings[i][0] < xMin) xMin = readings[i][0];
   //    if (readings[i][0] > xMax) xMax = readings[i][0];
   //    if (readings[i][1] < yMin) yMin = readings[i][1];
   //    if (readings[i][1] > yMax) yMax = readings[i][1];
   // }
   // xBias = (xMin+xMax)/2;
   // yBias = (yMin+yMax)/2;
   xBias = -27.5;
   yBias = +20.0;
   // xScale = xMax/yMax;
   // printf("%f, %f, %f", xMax, yMax, xScale);
}

void lsm303dlh_read_mag(mag_t *mag) {
   uint8_t buffer[6];
   int16_t magnet[3];
   uint8_t reg = MAG_REG;
   i2c_write_blocking( i2c_default, INTERFACE_B, &reg, 1, true );
   i2c_read_blocking( i2c_default, INTERFACE_B, buffer, 6, false );
   for (int i = 0; i < 3; i++) {
      magnet[i] = ((buffer[i * 2] << 8 | buffer[(i * 2) + 1] ));
   }
   // mag->x = (magnet[0] - xBias) * xScale; //OUT_X_M: 0x03 & 0x04 -20.0
   mag->x = magnet[0] + xBias; //OUT_X_M: 0x03 & 0x04 -20.0
   mag->y = magnet[2] + yBias; //OUT_Y_M: 0x07 & 0x08 +20.0
   mag->z = magnet[1];         //OUT_Z_M: 0x05 & 0x06
}

float get_angle(mag_t *mag) {
   // Raw Heading
   float heading = 1 - (float) atan2( (float)mag->y, (float)mag->x );

   // Correct for Magnetic Declination
   // https://www.magnetic-declination.com/SINGAPORE/SINGAPORE/2443440.html
   float decangl = (0.05 * M_PI) / 180.0;
   heading += decangl;

   // Correct for Radian Range
   if (heading < 0) heading += 2*M_PI;
   if (heading > 2*M_PI) heading -= 2*M_PI;

   // Convert from Radians to Degrees
   float rawHeadingDeg = (heading * 180.0) / M_PI;

   // Offset for Degrees Accuracy
   float newHeadingDeg = rawHeadingDeg;

   // Correct for Degrees Range
   if (newHeadingDeg < 0.0) newHeadingDeg += 360.0;
   if (newHeadingDeg > 360.0) newHeadingDeg -= 360.0;

   return newHeadingDeg;
}

// int main() {
//    mag_t mag;
//    stdio_init_all();
//    init_i2c_default();
//    lsm303dlh_mag_setup();
//    lsm303dlh_mag_hard_iron();
//    while (true) {
//       lsm303dlh_read_mag(&mag);
//       float angle = get_angle(&mag);
//       printf("%4d, %4d, %f\r\n", mag.x,mag.y,angle);
//       sleep_ms(REFRESH_PERIOD);
//    }
//    return 0;
// }

void init_i2c_default() {
   i2c_init(i2c_default, I2C_BAUD * 1000);
   gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
   gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
   gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
   gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}
