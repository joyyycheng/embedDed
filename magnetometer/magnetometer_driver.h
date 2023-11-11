#ifndef __LSM303DLH__
#define __LSM303DLH__
#include <stdint.h>
#define INTERFACE_B 0x1E // Magnetometr slave address
#define MAG_REG 0x03 // data starts on 0x03 - MSb set to 1 (autoincrease on read) -> 0xA8
#define MAG_CRA 0x00
#define MAG_CRB 0x01
#define MAG_MR 0x02

typedef struct {
   int16_t x;
   int16_t y;
   int16_t z;
} mag_t;

/*
 * Wake up and set the device (see IC documentation for more info)
 * (Documentation: https://cz.mouser.com/datasheet/2/389/stmicroelectronics_cd00260288-1206307.pdf)
 */
void lsm303dlh_mag_setup();

/*
 * Performs a sequention of i2c communication with INTERFACE_A address
 * and writes in given pointer to mag_t struct
 */
void lsm303dlh_read_mag(mag_t *mag);

/*
 * Returns a heading angle of the IC
 *
 */
float get_angle(mag_t *mag);

void magnetometer_init();
float magnetometer_heading();

#endif
/* end of lsm303dlh.h */
