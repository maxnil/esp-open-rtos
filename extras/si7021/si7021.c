/**
 * Copyright (c) 2018 Max Nilsson
 */

#include <stddef.h>
#include "si7021.h"

#ifdef SI7021_DEBUG
#include <stdio.h>
#define debug(fmt, ...) printf("%s" fmt "\n", "si7021: ", ## __VA_ARGS__);
#else
#define debug(fmt, ...)
#endif
#include "FreeRTOS.h"
#include "task.h"



static bool read_register(i2c_dev_t *dev, uint16_t addr, uint8_t *data, uint8_t len)
{
    uint8_t reg[2];
    uint8_t addr_size;

    if (addr > 0XFF) {
        addr_size = 2;
        reg[0] = addr>>8;
        reg[1] = addr & 0xFF;
    } else {
        addr_size = 1;
        reg[0] = addr;
        reg[1] = 0x00;
    }
    if (i2c_slave_write(dev->bus, dev->addr, NULL, reg, addr_size)) {
        debug("SI7021 i2c write failed");
        return false;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    if (i2c_slave_read(dev->bus, dev->addr, NULL, data, len)) {
        debug("SI7021 i2c read failed");
        return false;
    }

    return true;
}

static inline int read_data(i2c_dev_t *dev, uint8_t addr, uint8_t *value, uint8_t len)
{
    return i2c_slave_read(dev->bus, dev->addr, &addr, value, len);
}



static int write_register8(i2c_dev_t *dev, uint8_t addr, uint8_t value)
{
    return i2c_slave_write(dev->bus, dev->addr, &addr, &value, 1);
}

bool si7021_init(si7021_t *dev) //, si7021_params_t *params)
{
    uint8_t d[8];

    if (dev->i2c_dev.addr != SI7021_I2C_ADDRESS) {
        debug("Invalid I2C address");
        return false;
    }

    // Get firmware version
    d[0] = SI7021_REG_FW_REV>>8;
    d[1] = SI7021_REG_FW_REV & 0xFF;
    if (i2c_slave_write(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 2)) {
        debug("SI7021 i2c write failed 1");
        return false;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    if (i2c_slave_read(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 1)) {
        debug("SI7021 i2c read failed 2");
        return false;
    }
    if (d[0] != 0x20) {
        debug("SI7021 unexpected FW version: 0x%.2X (0x20)", d[0]);
        return false;
    }

    // Get ID2
    d[0] = SI7021_REG_ID2>>8;
    d[1] = SI7021_REG_ID2 & 0xFF;
    if (i2c_slave_write(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 2)) {
        debug("SI7021 i2c write failed 5");
        return false;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    if (i2c_slave_read(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 6)) {
        debug("SI7021 i2c read failed 6");
        return false;
    }
    if (d[0] != 0x15) {
        debug("SI7021 unexpected device ID 0x%.2X (0x15)", d[0]);
        return false;
    }

    return true;
}


bool si7021_read_float(si7021_t *dev, float *temperature, float *humidity) {
    uint8_t d[3];
    float data;
    
    // Humidity
    d[0] = SI7021_REG_MEAS_RH;//_HOLD;
    if (i2c_slave_write(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 1)) {
        debug("SI7021 i2c write failed 1");
        return false;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (i2c_slave_read(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 3)) {
        debug("SI7021 i2c read failed 2");
        return false;
    }

//    printf("SI7021 raw humid: 0x%.2X%.2X 0x%.2X\n", d[0], d[1], d[2]);
    
    data = ((125.0 * (d[0]<<8 | d[1])) / 65536.0) - 6.0;

    // Saturate
    if (data > 100.0)
        data = 100.0;
    if (data < 0.0)
        data = 0.0;
    
    *humidity = data;

    // Temperature
    d[0] = SI7021_REG_READ_T;
    if (i2c_slave_write(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 1)) {
        debug("SI7021 i2c write failed 3");
        return false;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    if (i2c_slave_read(dev->i2c_dev.bus, dev->i2c_dev.addr, NULL, d, 2)) {
        debug("SI7021 i2c read failed 4");
        return false;
    }

    data = (175.72 * (d[0]<<8 | d[1]) / 65536.0) - 46.85;
    *temperature = data;
    
    return true;
}
