/**
 * Copyright (c) 2018 Max Nilsson
 *
 */
#ifndef __SI7021_H__
#define __SI7021_H__

#include <stdint.h>
#include <stdbool.h>
#include "i2c/i2c.h"

/**
 * Uncomment to enable debug output.
 */
#define SI7021_DEBUG

#define SI7021_I2C_ADDRESS 0x40

#define SI7021_REG_MEAS_RH_HOLD 0xE5
#define SI7021_REG_MEAS_RH      0xF5
#define SI7021_REG_MEAS_T_HOLD  0xF3
#define SI7021_REG_MEAS_T       0xE3
#define SI7021_REG_READ_T       0xE0
#define SI7021_REG_RESET        0xFE
#define SI7021_REG_W_RHT        0xE6
#define SI7021_REG_R_RHT        0xE7
#define SI7021_REG_W_HEAT       0x51
#define SI7021_REG_R_HEAT       0x11
#define SI7021_REG_ID1          0xFA0F 
#define SI7021_REG_ID2          0xFCC9
#define SI7021_REG_FW_REV       0x84B8

/**
 * Configuration parameters for SI7021 module.
 */
typedef struct {
    int dummy;
} si7021_params_t;

typedef struct {
    i2c_dev_t  i2c_dev;  /* I2C dev setting. */
    uint8_t  id;         /* Chip ID */
} si7021_t;

/**
 */
bool si7021_init(si7021_t *dev); //, si7021_params_t *params);


/**
 * Read compensated temperature and pressure data:
 *  Temperature in degrees Celsius.
 *  Pressure in Pascals.
 *  Humidity is optional and only read for the BME280, in percent relative
 *  humidity.
 */
bool si7021_read_float(si7021_t *dev, float *temperature, float *humidity);

#endif  // __SI7021_H__
