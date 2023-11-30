#ifndef __LSM303DLH__
#define __LSM303DLH__ // Ensure Magnetometer Called Once.

#include <stdint.h>
#define MAG_SLV 0x1E // Magnetometer slave address.
#define MAG_OUT 0x03 // Magnetic field data address.
#define MAG_CRA 0x00 // CRA_REG_M register: Data output rate bits. These bits set the rate at which data is written to all three data output registers.
#define MAG_CRB 0x01 // CRB_REG_M register: Gain configuration bits. The gain configuration is common for all channels.
#define MAG_MRR 0x02 // MR_REG_M register : Mode select bits. These bits select the operation mode of this device.

// Structure for Magnetometer Coordinates
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} mag_t;

// Magnetometer Functions
void lsm303dlh_mag_setup(); // Setup Function
void lsm303dlh_read_mag(mag_t *mag); // Read Function
float lsm303dlh_get_angle(mag_t *mag); // Calculate Angle Function

void magnetometer_init(); // Initialise Sensor Function
float magnetometer_heading(); // Output Reading Function

#endif
