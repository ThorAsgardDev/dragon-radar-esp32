
#include "i2c.h"

#include "pins.h"

#include <stdio.h>
#include "hal/i2c_ll.h"


#define TIMEOUT_MS 50


struct s_i2c_ctx {
    i2c_port_t i2c_port;
};


void i2c_create(I2C_CTX *i2c_ctx) {
    struct s_i2c_ctx *ctx = (struct s_i2c_ctx *)malloc(sizeof(struct s_i2c_ctx));

    *i2c_ctx = ctx;

    memset(ctx, 0, sizeof(struct s_i2c_ctx));

    ctx->i2c_port = I2C_NUM_0;

    i2c_config_t i2c_config;
    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.scl_io_num = SCL;
    i2c_config.sda_io_num = SDA;
    i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.master.clk_speed = 1000000;
    i2c_config.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    ESP_ERROR_CHECK(i2c_param_config(ctx->i2c_port, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(ctx->i2c_port, i2c_config.mode, 0, 0, 0));
    ESP_ERROR_CHECK(i2c_set_timeout(ctx->i2c_port, I2C_LL_MAX_TIMEOUT));
}

void i2c_delete(I2C_CTX ctx) {
    free(ctx);
}

void i2c_write(I2C_CTX ctx, uint16_t address, uint8_t *data, uint32_t data_len) {
    i2c_cmd_handle_t cmd = NULL;

    uint8_t cmd_buffer[I2C_LINK_RECOMMENDED_SIZE(1)] = { 0 };

    cmd = i2c_cmd_link_create_static(cmd_buffer, I2C_LINK_RECOMMENDED_SIZE(1));
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true));

    if (data_len > 0) {
        ESP_ERROR_CHECK(i2c_master_write(cmd, data, data_len, true));
    }

    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(ctx->i2c_port, cmd, TIMEOUT_MS / portTICK_PERIOD_MS));

    if (cmd != NULL) {
        i2c_cmd_link_delete_static(cmd);
    }
}

void i2c_read(I2C_CTX ctx, uint16_t address, uint8_t *read_data, uint32_t read_data_len) {
    ESP_ERROR_CHECK(i2c_master_read_from_device(ctx->i2c_port, address, read_data, read_data_len, TIMEOUT_MS / portTICK_PERIOD_MS));
}
