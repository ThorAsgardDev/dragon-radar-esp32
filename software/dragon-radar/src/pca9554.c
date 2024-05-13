
#include "pca9554.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>


#define PCA9554_ADDRESS 0x3F

#define PCA9554_INPUT_PORT_REG 0x00
#define PCA9554_OUTPUT_PORT_REG 0x01
#define PCA9554_INVERSION_PORT_REG 0x02
#define PCA9554_CONFIG_PORT_REG 0x03

#define PCA_TFT_RESET 2
#define PCA_TFT_CS 1
#define PCA_TFT_SCK 0
#define PCA_TFT_MOSI 7


struct s_pca9554_ctx {
    I2C_CTX i2c_ctx;
    uint8_t config_port;
    uint8_t output_port;
};


void write_reg(PCA9554_CTX ctx, uint8_t reg, uint8_t data);
void read_reg(PCA9554_CTX ctx, uint8_t reg, uint8_t *data);
void begin_write_command(PCA9554_CTX ctx);
void end_write_command(PCA9554_CTX ctx);
void write_command(PCA9554_CTX ctx, uint8_t command, uint8_t last_databit);


void write_reg(PCA9554_CTX ctx, uint8_t reg, uint8_t data) {
    uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = data;
    i2c_write(ctx->i2c_ctx, PCA9554_ADDRESS, buffer, 2);
}

void read_reg(PCA9554_CTX ctx, uint8_t reg, uint8_t *data) {
    i2c_write(ctx->i2c_ctx, PCA9554_ADDRESS, &reg, 1);
    i2c_read(ctx->i2c_ctx, PCA9554_ADDRESS, data, 1);
}

void begin_write_command(PCA9554_CTX ctx) {
    pca9554_digital_write(ctx, PCA_TFT_CS, LOW);
}

void end_write_command(PCA9554_CTX ctx) {
    pca9554_digital_write(ctx, PCA_TFT_CS, HIGH);
}

void write_command(PCA9554_CTX ctx, uint8_t command, uint8_t last_databit) {
    pca9554_digital_write(ctx, PCA_TFT_MOSI, last_databit);
    pca9554_digital_write(ctx, PCA_TFT_SCK, LOW);
    pca9554_digital_write(ctx, PCA_TFT_SCK, HIGH);

    uint8_t bit = 0x80;
    while (bit) {
        if (command & bit) {
            if (last_databit != 1) {
                last_databit = 1;
                pca9554_digital_write(ctx, PCA_TFT_MOSI, HIGH);
            }
        } else {
            if (last_databit != 0) {
                last_databit = 0;
                pca9554_digital_write(ctx, PCA_TFT_MOSI, LOW);
            }
        }
        pca9554_digital_write(ctx, PCA_TFT_SCK, LOW);
        bit >>= 1;
        pca9554_digital_write(ctx, PCA_TFT_SCK, HIGH);
    }
}

void pca9554_create(PCA9554_CTX *pca9554_ctx, I2C_CTX i2c_ctx) {
    struct s_pca9554_ctx *ctx = (struct s_pca9554_ctx *)malloc(sizeof(struct s_pca9554_ctx));

    *pca9554_ctx = ctx;

    memset(ctx, 0, sizeof(struct s_pca9554_ctx));

    ctx->i2c_ctx = i2c_ctx;
    ctx->config_port = 0xff;
    ctx->output_port = 0;

    i2c_write(i2c_ctx, PCA9554_ADDRESS, NULL, 0);

    write_reg(ctx, PCA9554_INVERSION_PORT_REG, 0); // no invert
    write_reg(ctx, PCA9554_CONFIG_PORT_REG, ctx->config_port); // all input
    write_reg(ctx, PCA9554_OUTPUT_PORT_REG, ctx->output_port); // all low

    pca9554_pin_mode(ctx, PCA_TFT_RESET, OUTPUT);
    pca9554_digital_write(ctx, PCA_TFT_RESET, LOW);
    delay(10);
    pca9554_digital_write(ctx, PCA_TFT_RESET, HIGH);
    delay(100);

    pca9554_pin_mode(ctx, PCA_TFT_CS, OUTPUT);
    pca9554_digital_write(ctx, PCA_TFT_CS, HIGH);
    pca9554_pin_mode(ctx, PCA_TFT_SCK, OUTPUT);
    pca9554_digital_write(ctx, PCA_TFT_SCK, HIGH);
    pca9554_pin_mode(ctx, PCA_TFT_MOSI, OUTPUT);
    pca9554_digital_write(ctx, PCA_TFT_MOSI, HIGH);
}

void pca9554_delete(PCA9554_CTX ctx) {
    free(ctx);
}

void pca9554_pin_mode(PCA9554_CTX ctx, uint8_t pin, uint8_t mode) {
    if (mode == OUTPUT) {
        ctx->config_port &= ~(1UL << pin);
    } else {
        ctx->config_port |= (1UL << pin);
    }
    write_reg(ctx, PCA9554_CONFIG_PORT_REG, ctx->config_port);
}

void pca9554_digital_write(PCA9554_CTX ctx, uint8_t pin, uint8_t val) {
    if (val == HIGH) {
        ctx->output_port |= (1UL << pin);
    } else {
        ctx->output_port &= ~(1UL << pin);
    }
    write_reg(ctx, PCA9554_OUTPUT_PORT_REG, ctx->output_port);
}

void pca9554_send_command(PCA9554_CTX ctx, uint8_t command) {
    begin_write_command(ctx);
    write_command(ctx, command, 0);
    end_write_command(ctx);
}

void pca9554_batch(PCA9554_CTX ctx, const uint8_t *operations, uint32_t len) {
    for (size_t i = 0; i < len; ++i) {
        switch (operations[i]) {
            case BEGIN_WRITE:
                begin_write_command(ctx);
                break;
            case WRITE_C8_D8:
                write_command(ctx, operations[++i], 0);
                write_command(ctx, operations[++i], 1);
                break;
            case WRITE_COMMAND_8:
                write_command(ctx, operations[++i], 0);
                break;
            case END_WRITE:
                end_write_command(ctx);
                break;
            case DELAY:
                delay(operations[++i]);
                break;
            default:
                printf("Unknown operation id at %d: %d\n", i, operations[i]);
                break;
        }
    }
}

uint8_t pca9554_digital_read(PCA9554_CTX ctx, uint8_t pin) {
    uint8_t port;
    read_reg(ctx, PCA9554_INPUT_PORT_REG, &port);
    return (port & (1UL << pin)) ? HIGH : LOW;
}
