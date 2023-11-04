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
#include <stdint.h>
#define INTERFACE_A 0x19 // Accelerometr slave address
#define INTERFACE_B 0x1E // Magnetometr slave address
#define CTRL_REG_1 0x20
#define CTRL_REG_4 0x23
#define ACC_REG 0xA8 // data starts on 0x28 - MSb set to 1 (autoincrease on read) -> 0xA8
#define MAG_REG 0x03 // data starts on 0x03 - MSb set to 1 (autoincrease on read) -> 0xA8
#define MAG_CRA 0x00
#define MAG_CRB 0x01
#define MAG_MR 0x02
#define PI 3.141592653

// functional declaration


// function --------------------------------------------------------------
// void lsm303dlh_acc_setup() {
//    uint8_t buffer[2] = {CTRL_REG_4, 0x00};
//    i2c_write_blocking( i2c_default, INTERFACE_A, buffer, 2, true );
//    buffer[0] = CTRL_REG_1;
//    buffer[1] = 0x27;
//    i2c_write_blocking( i2c_default, INTERFACE_A, buffer, 2, false );
// }

// function --------------------------------------------------------------
void lsm303dlh_mag_setup() {
   uint8_t buffer[2];
   buffer[0] = MAG_CRA; buffer[1] = 0x10;
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, true );
   buffer[0] = MAG_CRB; buffer[1] = 0x20;
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, true );
   buffer[0] = MAG_MR;  buffer[1] = 0x00;
   i2c_write_blocking( i2c_default, INTERFACE_B, buffer, 2, false );
}

// function --------------------------------------------------------------
// void lsm303dlh_read_acc(accel_t *acc) {
//    uint8_t buffer[6];
//    int16_t accel[3];
//    uint8_t reg = ACC_REG;
//    i2c_write_blocking( i2c_default, INTERFACE_A, &reg, 1, true );
//    i2c_read_blocking( i2c_default, INTERFACE_A, buffer,  6, false );
//    for (int i = 0; i < 3; i++) {
//       accel[i] = ((buffer[i * 2] | buffer[(i * 2) + 1]  << 8));
//    }
//    acc->x = accel[0];
//    acc->y = accel[1];
//    acc->z = accel[2];
// }

// function --------------------------------------------------------------
void lsm303dlh_read_mag(mag_t *mag) {
   uint8_t buffer[6];
   int16_t magnet[3];
   uint8_t reg = MAG_REG;
   i2c_write_blocking( i2c_default, INTERFACE_B, &reg, 1, true );
   i2c_read_blocking( i2c_default, INTERFACE_B, buffer, 6, false );
   for (int i = 0; i < 3; i++) {
      magnet[i] = ((buffer[i * 2] << 8 | buffer[(i * 2) + 1] ));
   }
   mag->x = magnet[0];
   mag->y = magnet[1];
   mag->z = magnet[2];
}

// function --------------------------------------------------------------
float get_angle(mag_t *mag) {
   float angle_deg = (float)((atan2(mag->z, mag->x) * 180.0) / PI);
   if (angle_deg < 0.0) angle_deg += 360.0;
   angle_deg -= 262.5;
   if (angle_deg < 0.0) angle_deg += 360.0;
   return angle_deg;
}

// // main ------------------------------------------------------------------
// int main() {
//    init_i2c_default();
//    stdio_init_all();

//    // accel_t acc;
//    mag_t mag;

//    // read
//    while (true) {
//       // lsm303dlh_acc_setup();
//       lsm303dlh_mag_setup();
//       // lsm303dlh_read_acc(&acc);
//       lsm303dlh_read_mag(&mag);
//       float angle = get_angle(&mag);
//       // printf("Acc. X = %5d Y = %5d, Z = %5d \t Mag. X = %4d Y = %4d, Z = %4d \t Angle = %f \r\n",
//       //          acc.x,acc.y,acc.z,mag.x,mag.y,mag.z,angle);
//       printf("Mag. X = %4d Y = %4d, Z = %4d \t Angle = %f \r\n",
//                mag.x,mag.y,mag.z,angle);
//       sleep_ms(REFRESH_PERIOD);
//    }

//    return 0;
// }

// // function --------------------------------------------------------------
void init_i2c_default() {
   i2c_init(i2c_default, I2C_BAUD * 1000);
   gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
   gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
   gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
   gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

/* end of main.c */
