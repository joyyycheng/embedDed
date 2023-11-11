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

void init_i2c_default();

void lsm303dlh_mag_setup() {
   uint8_t buffer[2];
   buffer[0] = MAG_CRA; buffer[1] = 0x18; // CRA_REG_M: 75Hz(0x18) 30Hz(0x14) 15Hz(0x10) Data Output Rate
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, true );
   buffer[0] = MAG_CRB; buffer[1] = 0xE0; // CRB_REG_M: Gain X,Y,Z at 230 LSB/Gauss
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, true );
   buffer[0] = MAG_MR;  buffer[1] = 0x00; //  MR_REG_M: Continuous-Conversion Mode
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, false );
}

void lsm303dlh_read_mag(mag_t *mag) {
   lsm303dlh_mag_setup();
   uint8_t buffer[6];
   int16_t magnet[3];
   uint8_t reg = MAG_REG;
   i2c_write_blocking( i2c_default, INTERFACE_B, &reg, 1, true );
   i2c_read_blocking( i2c_default, INTERFACE_B, buffer, 6, false );
   for (int i = 0; i < 3; i++) {
      magnet[i] = ((buffer[i * 2] << 8 | buffer[(i * 2) + 1] ));
   }
   mag->x = magnet[0] - 67.5;  //OUT_X_M: 0x03 & 0x04
   mag->y = magnet[2] + 107.0; //OUT_Y_M: 0x07 & 0x08
   mag->z = magnet[1];         //OUT_Z_M: 0x05 & 0x06
}

// float get_angle(mag_t *mag) {
//    float angle_deg = (float)((atan2(mag->x, mag->y) * 180.0) / PI);
//    if (angle_deg < 0.0) angle_deg += 360.0;
//    // angle_deg -= 262.5;
//    // if (angle_deg < 0.0) angle_deg += 360.0;
//    return angle_deg;
// }

float get_angle(mag_t *mag) {
   // Raw Heading
   float_t heading = (float)(atan2(mag->x, mag->y));

   // Correct for Magnetic Declination
   float_t decangl = 0.05;
   heading += decangl;

   // Correct for Radian Range
   if (heading < 0) heading += 2*M_PI;
   if (heading > 2*M_PI) heading -= 2*M_PI;

   // Convert from Radians to Degrees
   float_t rawHeadingDeg = heading * 180.0/M_PI;

   // Offset for Degrees Accuracy
   float_t newHeadingDeg = rawHeadingDeg + 17.0;
   if (rawHeadingDeg >= 0.0 && rawHeadingDeg < 45.0)    newHeadingDeg += 0.0;
   if (rawHeadingDeg >= 45.0 && rawHeadingDeg < 90.0)   newHeadingDeg += 1.0;
   if (rawHeadingDeg >= 90.0 && rawHeadingDeg < 135.0)  newHeadingDeg += 2.0;
   if (rawHeadingDeg >= 135.0 && rawHeadingDeg < 180.0) newHeadingDeg += 3.0;
   if (rawHeadingDeg >= 180.0 && rawHeadingDeg < 225.0) newHeadingDeg += 3.0;
   if (rawHeadingDeg >= 225.0 && rawHeadingDeg < 270.0) newHeadingDeg += 2.0;
   if (rawHeadingDeg >= 270.0 && rawHeadingDeg < 315.0) newHeadingDeg += 1.0;
   if (rawHeadingDeg >= 315.0 && rawHeadingDeg < 360.0) newHeadingDeg += 0.0;

   // Correct for Degrees Range
   if (newHeadingDeg < 0.0) newHeadingDeg += 360.0;
   if (newHeadingDeg > 360.0) newHeadingDeg -= 360.0;

   return newHeadingDeg;
}

// int main() {
//    mag_t mag;
//    init_i2c_default();
//    stdio_init_all();
//    while (true) {
//       lsm303dlh_read_mag(&mag);
//       float angle = get_angle(&mag);
//       printf("%4d, %4d, %4d, %f \r\n", mag.x,mag.y,mag.z,angle);
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
