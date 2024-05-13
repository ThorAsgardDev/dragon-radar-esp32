
#ifndef __I2C_H__
#define __I2C_H__


#include <inttypes.h>
#include "hal/i2c_types.h"
#include "driver/i2c.h"


typedef struct s_i2c_ctx *I2C_CTX;


void i2c_create(I2C_CTX *ctx);
void i2c_delete(I2C_CTX ctx);
void i2c_write(I2C_CTX ctx, uint16_t address, uint8_t *data, uint32_t data_len);
void i2c_read(I2C_CTX ctx, uint16_t address, uint8_t *read_data, uint32_t read_data_len);


#endif