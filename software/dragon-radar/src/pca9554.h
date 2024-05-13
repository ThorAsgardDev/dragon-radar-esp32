
#ifndef __PCA9554_H__
#define __PCA9554_H__


#include "i2c.h"

#include <inttypes.h>


#define LOW 0x0
#define HIGH 0x1

#define OUTPUT 0x03

#define PCA_TFT_BACKLIGHT 4
#define PCA_BUTTON_UP 5
#define PCA_BUTTON_DOWN 6


typedef struct s_pca9554_ctx *PCA9554_CTX;

typedef enum
{
  BEGIN_WRITE,
  WRITE_COMMAND_8,
  WRITE_C8_D8,
  END_WRITE,
  DELAY,
} spi_operation_type_t;


void pca9554_create(PCA9554_CTX *ctx, I2C_CTX i2c_ctx);
void pca9554_delete(PCA9554_CTX ctx);
void pca9554_pin_mode(PCA9554_CTX ctx, uint8_t pin, uint8_t mode);
void pca9554_digital_write(PCA9554_CTX ctx, uint8_t pin, uint8_t val);
void pca9554_send_command(PCA9554_CTX ctx, uint8_t command);
void pca9554_batch(PCA9554_CTX ctx, const uint8_t *operations, uint32_t len);
uint8_t pca9554_digital_read(PCA9554_CTX ctx, uint8_t pin);


#endif