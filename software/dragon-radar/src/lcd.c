
#include "lcd.h"

#include "pins.h"
#include "utils.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_err.h"


#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720


bool on_bounce_empty(esp_lcd_panel_handle_t panel, void *bounce_buf, int pos_px, int len_bytes, void *user_ctx);
bool on_vsync(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *data, void *user_ctx);


struct s_lcd_ctx {
    // Semaphore used to synchronize frame buffers swap
    SemaphoreHandle_t sem_vsync_end;
    SemaphoreHandle_t sem_frame_buffer_ready;

    esp_lcd_panel_handle_t panel_handle;
    uint16_t *frame_buffers[2];
    volatile int current_frame_buffer_idx;
    int width;
    int height;
};


/*static const uint8_t hd40015c40_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x01,
    WRITE_C8_D8, 0xE3, 0x00,
    WRITE_C8_D8, 0x0A, 0x11,
    WRITE_C8_D8, 0x23, 0xA0,
    WRITE_C8_D8, 0x24, 0x32,
    WRITE_C8_D8, 0x25, 0x12,
    WRITE_C8_D8, 0x26, 0x2E,
    WRITE_C8_D8, 0x27, 0x2E,
    WRITE_C8_D8, 0x29, 0x02,
    WRITE_C8_D8, 0x2A, 0xCF,
    WRITE_C8_D8, 0x32, 0x34,
    WRITE_C8_D8, 0x38, 0x9C,
    WRITE_C8_D8, 0x39, 0xA7,
    WRITE_C8_D8, 0x3A, 0x27,
    WRITE_C8_D8, 0x3B, 0x94,
    WRITE_C8_D8, 0x42, 0x6D,
    WRITE_C8_D8, 0x43, 0x83,
    WRITE_C8_D8, 0x81, 0x00,
    WRITE_C8_D8, 0x91, 0x67,
    WRITE_C8_D8, 0x92, 0x67,
    WRITE_C8_D8, 0xA0, 0x52,
    WRITE_C8_D8, 0xA1, 0x50,
    WRITE_C8_D8, 0xA4, 0x9C,
    WRITE_C8_D8, 0xA7, 0x02,
    WRITE_C8_D8, 0xA8, 0x02,
    WRITE_C8_D8, 0xA9, 0x02,
    WRITE_C8_D8, 0xAA, 0xA8,
    WRITE_C8_D8, 0xAB, 0x28,
    WRITE_C8_D8, 0xAE, 0xD2,
    WRITE_C8_D8, 0xAF, 0x02,
    WRITE_C8_D8, 0xB0, 0xD2,
    WRITE_C8_D8, 0xB2, 0x26,
    WRITE_C8_D8, 0xB3, 0x26,

    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x02,
    WRITE_C8_D8, 0xB1, 0x0A,
    WRITE_C8_D8, 0xD1, 0x0E,
    WRITE_C8_D8, 0xB4, 0x2F,
    WRITE_C8_D8, 0xD4, 0x2D,
    WRITE_C8_D8, 0xB2, 0x0C,
    WRITE_C8_D8, 0xD2, 0x0C,
    WRITE_C8_D8, 0xB3, 0x30,
    WRITE_C8_D8, 0xD3, 0x2A,
    WRITE_C8_D8, 0xB6, 0x1E,
    WRITE_C8_D8, 0xD6, 0x16,
    WRITE_C8_D8, 0xB7, 0x3B,
    WRITE_C8_D8, 0xD7, 0x35,
    WRITE_C8_D8, 0xC1, 0x08,
    WRITE_C8_D8, 0xE1, 0x08,
    WRITE_C8_D8, 0xB8, 0x0D,
    WRITE_C8_D8, 0xD8, 0x0D,
    WRITE_C8_D8, 0xB9, 0x05,
    WRITE_C8_D8, 0xD9, 0x05,
    WRITE_C8_D8, 0xBD, 0x15,
    WRITE_C8_D8, 0xDD, 0x15,
    WRITE_C8_D8, 0xBC, 0x13,
    WRITE_C8_D8, 0xDC, 0x13,
    WRITE_C8_D8, 0xBB, 0x12,
    WRITE_C8_D8, 0xDB, 0x10,
    WRITE_C8_D8, 0xBA, 0x11,
    WRITE_C8_D8, 0xDA, 0x11,
    WRITE_C8_D8, 0xBE, 0x17,
    WRITE_C8_D8, 0xDE, 0x17,
    WRITE_C8_D8, 0xBF, 0x0F,
    WRITE_C8_D8, 0xDF, 0x0F,
    WRITE_C8_D8, 0xC0, 0x16,
    WRITE_C8_D8, 0xE0, 0x16,
    WRITE_C8_D8, 0xB5, 0x2E,
    WRITE_C8_D8, 0xD5, 0x3F,
    WRITE_C8_D8, 0xB0, 0x03,
    WRITE_C8_D8, 0xD0, 0x02,

    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x03,
    WRITE_C8_D8, 0x08, 0x09,
    WRITE_C8_D8, 0x09, 0x0A,
    WRITE_C8_D8, 0x0A, 0x0B,
    WRITE_C8_D8, 0x0B, 0x0C,
    WRITE_C8_D8, 0x28, 0x22,
    WRITE_C8_D8, 0x2A, 0xE9,
    WRITE_C8_D8, 0x2B, 0xE9,
    WRITE_C8_D8, 0x34, 0x51,
    WRITE_C8_D8, 0x35, 0x01,
    WRITE_C8_D8, 0x36, 0x26,
    WRITE_C8_D8, 0x37, 0x13,
    WRITE_C8_D8, 0x40, 0x07,
    WRITE_C8_D8, 0x41, 0x08,
    WRITE_C8_D8, 0x42, 0x09,
    WRITE_C8_D8, 0x43, 0x0A,
    WRITE_C8_D8, 0x44, 0x22,
    WRITE_C8_D8, 0x45, 0xDB,
    WRITE_C8_D8, 0x46, 0xdC,
    WRITE_C8_D8, 0x47, 0x22,
    WRITE_C8_D8, 0x48, 0xDD,
    WRITE_C8_D8, 0x49, 0xDE,
    WRITE_C8_D8, 0x50, 0x0B,
    WRITE_C8_D8, 0x51, 0x0C,
    WRITE_C8_D8, 0x52, 0x0D,
    WRITE_C8_D8, 0x53, 0x0E,
    WRITE_C8_D8, 0x54, 0x22,
    WRITE_C8_D8, 0x55, 0xDF,
    WRITE_C8_D8, 0x56, 0xE0,
    WRITE_C8_D8, 0x57, 0x22,
    WRITE_C8_D8, 0x58, 0xE1,
    WRITE_C8_D8, 0x59, 0xE2,
    WRITE_C8_D8, 0x80, 0x1E,
    WRITE_C8_D8, 0x81, 0x1E,
    WRITE_C8_D8, 0x82, 0x1F,
    WRITE_C8_D8, 0x83, 0x1F,
    WRITE_C8_D8, 0x84, 0x05,
    WRITE_C8_D8, 0x85, 0x0A,
    WRITE_C8_D8, 0x86, 0x0A,
    WRITE_C8_D8, 0x87, 0x0C,
    WRITE_C8_D8, 0x88, 0x0C,
    WRITE_C8_D8, 0x89, 0x0E,
    WRITE_C8_D8, 0x8A, 0x0E,
    WRITE_C8_D8, 0x8B, 0x10,
    WRITE_C8_D8, 0x8C, 0x10,
    WRITE_C8_D8, 0x8D, 0x00,
    WRITE_C8_D8, 0x8E, 0x00,
    WRITE_C8_D8, 0x8F, 0x1F,
    WRITE_C8_D8, 0x90, 0x1F,
    WRITE_C8_D8, 0x91, 0x1E,
    WRITE_C8_D8, 0x92, 0x1E,
    WRITE_C8_D8, 0x93, 0x02,
    WRITE_C8_D8, 0x94, 0x04,
    WRITE_C8_D8, 0x96, 0x1E,
    WRITE_C8_D8, 0x97, 0x1E,
    WRITE_C8_D8, 0x98, 0x1F,
    WRITE_C8_D8, 0x99, 0x1F,
    WRITE_C8_D8, 0x9A, 0x05,
    WRITE_C8_D8, 0x9B, 0x09,
    WRITE_C8_D8, 0x9C, 0x09,
    WRITE_C8_D8, 0x9D, 0x0B,
    WRITE_C8_D8, 0x9E, 0x0B,
    WRITE_C8_D8, 0x9F, 0x0D,
    WRITE_C8_D8, 0xA0, 0x0D,
    WRITE_C8_D8, 0xA1, 0x0F,
    WRITE_C8_D8, 0xA2, 0x0F,
    WRITE_C8_D8, 0xA3, 0x00,
    WRITE_C8_D8, 0xA4, 0x00,
    WRITE_C8_D8, 0xA5, 0x1F,
    WRITE_C8_D8, 0xA6, 0x1F,
    WRITE_C8_D8, 0xA7, 0x1E,
    WRITE_C8_D8, 0xA8, 0x1E,
    WRITE_C8_D8, 0xA9, 0x01,
    WRITE_C8_D8, 0xAA, 0x03,

    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x00,
    WRITE_C8_D8, 0x36, 0x0A,

    WRITE_COMMAND_8, 0x11, // Sleep Out
    END_WRITE,

    DELAY, 100,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x29, // Display On
    END_WRITE,

    DELAY, 50
};*/

static const uint8_t hd40015c40_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x01,
    WRITE_C8_D8, 0xE3, 0x00,
    WRITE_C8_D8, 0x0A, 0x11,
    WRITE_C8_D8, 0x23, 0xA0, // 0xA2
    WRITE_C8_D8, 0x24, 0x32,
    WRITE_C8_D8, 0x25, 0x12,
    WRITE_C8_D8, 0x26, 0x2E,
    WRITE_C8_D8, 0x27, 0x2E,
    
    WRITE_C8_D8, 0x29, 0x02,
    WRITE_C8_D8, 0x2A, 0xCF,
    WRITE_C8_D8, 0x32, 0x34,
    WRITE_C8_D8, 0x38, 0x9C,
    WRITE_C8_D8, 0x39, 0xA7,
    WRITE_C8_D8, 0x3A, 0x27,
    WRITE_C8_D8, 0x3B, 0x94,
    WRITE_C8_D8, 0x42, 0x6D,
    WRITE_C8_D8, 0x43, 0x83,
    WRITE_C8_D8, 0x81, 0x00,
    WRITE_C8_D8, 0x91, 0x67,
    WRITE_C8_D8, 0x92, 0x67,
    WRITE_C8_D8, 0xA0, 0x52,
    WRITE_C8_D8, 0xA1, 0x50,
    WRITE_C8_D8, 0xA4, 0x9C,
    WRITE_C8_D8, 0xA7, 0x02,
    WRITE_C8_D8, 0xA8, 0x02,
    WRITE_C8_D8, 0xA9, 0x02,
    WRITE_C8_D8, 0xAA, 0xA8,
    WRITE_C8_D8, 0xAB, 0x28,
    WRITE_C8_D8, 0xAE, 0xD2,
    WRITE_C8_D8, 0xAF, 0x02,
    WRITE_C8_D8, 0xB0, 0xD2,
    WRITE_C8_D8, 0xB2, 0x26,
    WRITE_C8_D8, 0xB3, 0x26,

    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x02,
    WRITE_C8_D8, 0xB1, 0x0A,
    WRITE_C8_D8, 0xD1, 0x0E,
    WRITE_C8_D8, 0xB4, 0x2F,
    WRITE_C8_D8, 0xD4, 0x2D,
    WRITE_C8_D8, 0xB2, 0x0C,
    WRITE_C8_D8, 0xD2, 0x0C,
    WRITE_C8_D8, 0xB3, 0x30,
    WRITE_C8_D8, 0xD3, 0x2A,
    WRITE_C8_D8, 0xB6, 0x1E,
    WRITE_C8_D8, 0xD6, 0x16,
    WRITE_C8_D8, 0xB7, 0x3B,
    WRITE_C8_D8, 0xD7, 0x35,
    WRITE_C8_D8, 0xC1, 0x08,
    WRITE_C8_D8, 0xE1, 0x08,
    WRITE_C8_D8, 0xB8, 0x0D,
    WRITE_C8_D8, 0xD8, 0x0D,
    WRITE_C8_D8, 0xB9, 0x05,
    WRITE_C8_D8, 0xD9, 0x05,
    WRITE_C8_D8, 0xBD, 0x15,
    WRITE_C8_D8, 0xDD, 0x15,
    WRITE_C8_D8, 0xBC, 0x13,
    WRITE_C8_D8, 0xDC, 0x13,
    WRITE_C8_D8, 0xBB, 0x12,
    WRITE_C8_D8, 0xDB, 0x10,
    WRITE_C8_D8, 0xBA, 0x11,
    WRITE_C8_D8, 0xDA, 0x11,
    WRITE_C8_D8, 0xBE, 0x17,
    WRITE_C8_D8, 0xDE, 0x17,
    WRITE_C8_D8, 0xBF, 0x0F,
    WRITE_C8_D8, 0xDF, 0x0F,
    WRITE_C8_D8, 0xC0, 0x16,
    WRITE_C8_D8, 0xE0, 0x16,
    WRITE_C8_D8, 0xB5, 0x2E,
    WRITE_C8_D8, 0xD5, 0x3F,
    WRITE_C8_D8, 0xB0, 0x03,
    WRITE_C8_D8, 0xD0, 0x02,

    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x03,
    WRITE_C8_D8, 0x08, 0x09,
    WRITE_C8_D8, 0x09, 0x0A,
    WRITE_C8_D8, 0x0A, 0x0B,
    WRITE_C8_D8, 0x0B, 0x0C,
    WRITE_C8_D8, 0x28, 0x22,
    WRITE_C8_D8, 0x2A, 0xE9,
    WRITE_C8_D8, 0x2B, 0xE9,
    WRITE_C8_D8, 0x34, 0x51,
    WRITE_C8_D8, 0x35, 0x01,
    WRITE_C8_D8, 0x36, 0x26,
    WRITE_C8_D8, 0x37, 0x13,
    WRITE_C8_D8, 0x40, 0x07,
    WRITE_C8_D8, 0x41, 0x08,
    WRITE_C8_D8, 0x42, 0x09,
    WRITE_C8_D8, 0x43, 0x0A,
    WRITE_C8_D8, 0x44, 0x22,
    WRITE_C8_D8, 0x45, 0xDB,
    WRITE_C8_D8, 0x46, 0xdC,
    WRITE_C8_D8, 0x47, 0x22,
    WRITE_C8_D8, 0x48, 0xDD,
    WRITE_C8_D8, 0x49, 0xDE,
    WRITE_C8_D8, 0x50, 0x0B,
    WRITE_C8_D8, 0x51, 0x0C,
    WRITE_C8_D8, 0x52, 0x0D,
    WRITE_C8_D8, 0x53, 0x0E,
    WRITE_C8_D8, 0x54, 0x22,
    WRITE_C8_D8, 0x55, 0xDF,
    WRITE_C8_D8, 0x56, 0xE0,
    WRITE_C8_D8, 0x57, 0x22,
    WRITE_C8_D8, 0x58, 0xE1,
    WRITE_C8_D8, 0x59, 0xE2,
    WRITE_C8_D8, 0x80, 0x1E,
    WRITE_C8_D8, 0x81, 0x1E,
    WRITE_C8_D8, 0x82, 0x1F,
    WRITE_C8_D8, 0x83, 0x1F,
    WRITE_C8_D8, 0x84, 0x05,
    WRITE_C8_D8, 0x85, 0x0A,
    WRITE_C8_D8, 0x86, 0x0A,
    WRITE_C8_D8, 0x87, 0x0C,
    WRITE_C8_D8, 0x88, 0x0C,
    WRITE_C8_D8, 0x89, 0x0E,
    WRITE_C8_D8, 0x8A, 0x0E,
    WRITE_C8_D8, 0x8B, 0x10,
    WRITE_C8_D8, 0x8C, 0x10,
    WRITE_C8_D8, 0x8D, 0x00,
    WRITE_C8_D8, 0x8E, 0x00,
    WRITE_C8_D8, 0x8F, 0x1F,
    WRITE_C8_D8, 0x90, 0x1F,
    WRITE_C8_D8, 0x91, 0x1E,
    WRITE_C8_D8, 0x92, 0x1E,
    WRITE_C8_D8, 0x93, 0x02,
    WRITE_C8_D8, 0x94, 0x04,
    WRITE_C8_D8, 0x96, 0x1E,
    WRITE_C8_D8, 0x97, 0x1E,
    WRITE_C8_D8, 0x98, 0x1F,
    WRITE_C8_D8, 0x99, 0x1F,
    WRITE_C8_D8, 0x9A, 0x05,
    WRITE_C8_D8, 0x9B, 0x09,
    WRITE_C8_D8, 0x9C, 0x09,
    WRITE_C8_D8, 0x9D, 0x0B,
    WRITE_C8_D8, 0x9E, 0x0B,
    WRITE_C8_D8, 0x9F, 0x0D,
    WRITE_C8_D8, 0xA0, 0x0D,
    WRITE_C8_D8, 0xA1, 0x0F,
    WRITE_C8_D8, 0xA2, 0x0F,
    WRITE_C8_D8, 0xA3, 0x00,
    WRITE_C8_D8, 0xA4, 0x00,
    WRITE_C8_D8, 0xA5, 0x1F,
    WRITE_C8_D8, 0xA6, 0x1F,
    WRITE_C8_D8, 0xA7, 0x1E,
    WRITE_C8_D8, 0xA8, 0x1E,
    WRITE_C8_D8, 0xA9, 0x01,
    WRITE_C8_D8, 0xAA, 0x03,

    WRITE_C8_D8, 0xFF, 0x30,
    WRITE_C8_D8, 0xFF, 0x52,
    WRITE_C8_D8, 0xFF, 0x00,
    WRITE_C8_D8, 0x36, 0x0A,

    WRITE_C8_D8, 0x11, 0x00,
    //WRITE_COMMAND_8, 0x11, // Sleep Out
    END_WRITE,

    DELAY, 200,

    BEGIN_WRITE,
    WRITE_C8_D8, 0x29, 0x00,
    //WRITE_COMMAND_8, 0x29, // Display On
    END_WRITE,

    DELAY, 100
};


/*bool on_bounce_empty(esp_lcd_panel_handle_t panel, void *bounce_buf, int pos_px, int len_bytes, void *user_ctx) {
    LCD_CTX ctx = (LCD_CTX)user_ctx;

    int x = pos_px % SCREEN_WIDTH;
    int y = pos_px / SCREEN_WIDTH;

    uint16_t *dst = bounce_buf;
    uint16_t *src = ctx->frame_buffers[1 - ctx->current_frame_buffer_idx] + (y >> 1) * ctx->width + (x >> 1);

    while (len_bytes > 0) {
        *dst = *src;
        dst++;

        x++;
        if (x & 1) {
            src++;
        }
        if (x >= SCREEN_WIDTH) {
            x = 0;
            y++;
            if (y & 1) {
                src -= ctx->width;
            }
        }
        len_bytes -= 2; // 2 bytes (RGB565)
    }

    return false;
}*/

bool on_bounce_empty(esp_lcd_panel_handle_t panel, void *bounce_buf, int pos_px, int len_bytes, void *user_ctx) {
    LCD_CTX ctx = (LCD_CTX)user_ctx;

    int x = SCREEN_WIDTH - (pos_px % SCREEN_WIDTH) - 1;
    int y = SCREEN_HEIGHT - (pos_px / SCREEN_WIDTH) - 1;

    uint16_t *dst = bounce_buf;
    uint16_t *src = ctx->frame_buffers[1 - ctx->current_frame_buffer_idx] + (y >> 1) * ctx->width + (x >> 1);

    while (len_bytes > 0) {
        *dst = *src;
        dst++;

        if (x & 1) {
            src--;
        }
        x--;
        if (x < 0) {
            x = SCREEN_WIDTH - 1;
            if (y & 1) {
                src += ctx->width;
            }
            y--;
        }
        len_bytes -= 2; // 2 bytes (RGB565)
    }

    return false;
}

bool on_vsync(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *data, void *user_ctx) {
    LCD_CTX ctx = (LCD_CTX)user_ctx;

    BaseType_t high_task_awoken0 = pdFALSE;
    xSemaphoreGiveFromISR(ctx->sem_vsync_end, &high_task_awoken0);

    BaseType_t high_task_awoken1 = pdFALSE;
    xSemaphoreTakeFromISR(ctx->sem_frame_buffer_ready, &high_task_awoken1);

    return (high_task_awoken0 == pdTRUE) || (high_task_awoken1 == pdTRUE);
}

void lcd_create(LCD_CTX *lcd_ctx, PCA9554_CTX pca9554_ctx) {
    struct s_lcd_ctx *ctx = (struct s_lcd_ctx *)malloc(sizeof(struct s_lcd_ctx));

    *lcd_ctx = ctx;

    memset(ctx, 0, sizeof(struct s_lcd_ctx));

    ctx->width = SCREEN_WIDTH >> 1;
    ctx->height = SCREEN_HEIGHT >> 1;
    // ctx->width = SCREEN_WIDTH;
    // ctx->height = SCREEN_HEIGHT;

    ctx->current_frame_buffer_idx = 0;

    ctx->sem_vsync_end = xSemaphoreCreateBinary();
    assert(ctx->sem_vsync_end);
    ctx->sem_frame_buffer_ready = xSemaphoreCreateBinary();
    assert(ctx->sem_frame_buffer_ready);

    for (int i = 0; i < 2; i++) {
        ctx->frame_buffers[i] = (uint16_t *)heap_caps_calloc(ctx->width * ctx->height, sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    }

    pca9554_send_command(pca9554_ctx, 0x01);
    delay(120);
    pca9554_batch(pca9554_ctx, hd40015c40_init_operations, sizeof(hd40015c40_init_operations));
    pca9554_pin_mode(pca9554_ctx, PCA_TFT_BACKLIGHT, OUTPUT);
    pca9554_digital_write(pca9554_ctx, PCA_TFT_BACKLIGHT, HIGH);

    esp_lcd_rgb_panel_config_t panel_config;

    memset(&panel_config, 0, sizeof(esp_lcd_rgb_panel_config_t));

    panel_config.data_width = 16; // RGB565 in parallel mode, thus 16bit in width
    panel_config.psram_trans_align = 64;
    panel_config.clk_src = LCD_CLK_SRC_PLL160M;
    panel_config.disp_gpio_num = GPIO_NUM_NC;
    panel_config.pclk_gpio_num = TFT_PCLK;
    panel_config.vsync_gpio_num = TFT_VSYNC;
    panel_config.hsync_gpio_num = TFT_HSYNC;
    panel_config.de_gpio_num = TFT_DE;
    panel_config.data_gpio_nums[0] = TFT_B1;
    panel_config.data_gpio_nums[1] = TFT_B2;
    panel_config.data_gpio_nums[2] = TFT_B3;
    panel_config.data_gpio_nums[3] = TFT_B4;
    panel_config.data_gpio_nums[4] = TFT_B5;
    panel_config.data_gpio_nums[5] = TFT_G0;
    panel_config.data_gpio_nums[6] = TFT_G1;
    panel_config.data_gpio_nums[7] = TFT_G2;
    panel_config.data_gpio_nums[8] = TFT_G3;
    panel_config.data_gpio_nums[9] = TFT_G4;
    panel_config.data_gpio_nums[10] = TFT_G5;
    panel_config.data_gpio_nums[11] = TFT_R1;
    panel_config.data_gpio_nums[12] = TFT_R2;
    panel_config.data_gpio_nums[13] = TFT_R3;
    panel_config.data_gpio_nums[14] = TFT_R4;
    panel_config.data_gpio_nums[15] = TFT_R5;

    panel_config.timings.pclk_hz = 17000000;
    panel_config.timings.h_res = SCREEN_WIDTH;
    panel_config.timings.v_res = SCREEN_HEIGHT;
    panel_config.timings.hsync_back_porch = 44;
    panel_config.timings.hsync_front_porch = 46;
    panel_config.timings.hsync_pulse_width = 2;
    panel_config.timings.vsync_back_porch = 16;
    panel_config.timings.vsync_front_porch = 50;
    panel_config.timings.vsync_pulse_width = 5;
    panel_config.timings.flags.pclk_active_neg = 0;

    panel_config.timings.flags.vsync_idle_low = 1;

    // panel_config.num_fbs = 2;
    // panel_config.flags.fb_in_psram = true;
    panel_config.flags.no_fb = true;
    panel_config.flags.fb_in_psram = false;
    panel_config.bounce_buffer_size_px = 10 * SCREEN_WIDTH;

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &ctx->panel_handle));

    esp_lcd_rgb_panel_event_callbacks_t callbacks;
    memset(&callbacks, 0, sizeof(esp_lcd_rgb_panel_event_callbacks_t));
    callbacks.on_bounce_empty = on_bounce_empty;
    // callbacks.on_vsync = on_vsync;
    callbacks.on_bounce_frame_finish = on_vsync;
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(ctx->panel_handle, &callbacks, ctx));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(ctx->panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(ctx->panel_handle));

    // ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(ctx->panel_handle, 2, &ctx->frame_buffers[0], &ctx->frame_buffers[1]));
}

void lcd_delete(LCD_CTX ctx) {
    free(ctx);
}

uint16_t *lcd_get_frame_buffer(LCD_CTX ctx) {
    return ctx->frame_buffers[ctx->current_frame_buffer_idx];
}

int lcd_get_width(LCD_CTX ctx) {
    return ctx->width;
}

int lcd_get_height(LCD_CTX ctx) {
    return ctx->height;
}

void lcd_update(LCD_CTX ctx) {
    xSemaphoreTake(ctx->sem_vsync_end, portMAX_DELAY);
    // ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(ctx->panel_handle, 0, 0, ctx->width, ctx->height, ctx->frame_buffers[ctx->current_frame_buffer_idx]));
    ctx->current_frame_buffer_idx = 1 - ctx->current_frame_buffer_idx;
    xSemaphoreGive(ctx->sem_frame_buffer_ready);
}
