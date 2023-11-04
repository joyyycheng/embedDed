

typedef struct {
   int16_t x;
   int16_t y;
   int16_t z;
} accel_t;

typedef struct {
   int16_t x;
   int16_t y;
   int16_t z;
} mag_t;

/*
 * Wake up and set the device (see IC documentation for more info)
 * (Documentation: https://cz.mouser.com/datasheet/2/389/stmicroelectronics_cd00260288-1206307.pdf)
 */
// void lsm303dlh_acc_setup();

/*
 * Wake up and set the device (see IC documentation for more info)
 * (Documentation: https://cz.mouser.com/datasheet/2/389/stmicroelectronics_cd00260288-1206307.pdf)
 */
void lsm303dlh_mag_setup();

/*
 * Performs a sequention of i2c communication with INTERFACE_A address
 * and writes in given pointer to acc_t struct
 */
// void lsm303dlh_read_acc(accel_t *acc);

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

void init_i2c_default();


/* end of lsm303dlh.h */
