
#include "pins.h"
#include "utils.h"
#include "i2c.h"
#include "pca9554.h"
#include "lcd.h"
#include "gfx.h"
#include "i2s.h"
#include "db_cluster_mngr.h"
#include "constants.h"

#include "resources/image_arrow.h"

#include <stdio.h>


void core0_task(void *pvParameter) {

    gpio_set_direction(PUSH_BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PUSH_BUTTON, GPIO_PULLUP_ONLY);
    
    I2C_CTX i2c_ctx;
    i2c_create(&i2c_ctx);

    PCA9554_CTX pca9554_ctx;
    pca9554_create(&pca9554_ctx, i2c_ctx);

    LCD_CTX lcd_ctx;
    lcd_create(&lcd_ctx, pca9554_ctx);

    GFX_CTX gfx_ctx;
    gfx_create(&gfx_ctx, lcd_ctx);

    DB_CLUSTER_MNGR_CTX db_cluster_mngr_ctx;
    db_cluster_mngr_create(&db_cluster_mngr_ctx, gfx_ctx);

    int zoom = MAX_ZOOM_OUT;

    int width = gfx_get_width(gfx_ctx);
    int height = gfx_get_height(gfx_ctx);

    while (true) {

        // gfx_fill_rect(gfx_ctx, 0, 0, width, height, RGB565(7, 132, 0));
        gfx_fill_rect(gfx_ctx, 0, 0, width, height, RGB565(7, 132, 0));

        int zone_count = 4 + zoom * 2;
        for (int i = 1; i < zone_count; i++) {
            gfx_draw_h_line(gfx_ctx, i * height / zone_count, 1);
            gfx_draw_v_line(gfx_ctx, i * width / zone_count, 1);
        }

        gfx_draw_image(gfx_ctx, (width >> 1) - (IMAGE_ARROW_W >> 1), (height >> 1) - (IMAGE_ARROW_H >> 1), image_arrow_data, IMAGE_ARROW_W, IMAGE_ARROW_H);

        bool playsound = db_cluster_mngr_update(db_cluster_mngr_ctx, zoom);
        if (playsound) {
            i2s_play_sound_bip();
        }

        gfx_update(gfx_ctx);

        static int previous_push_button = 1;
        int current_push_button = gpio_get_level(PUSH_BUTTON);
        if (previous_push_button != current_push_button && current_push_button == 0) {
            i2s_play_sound_button();

            zoom--;
            if (zoom < 0) {
                zoom = MAX_ZOOM_OUT;
            }
        }
        previous_push_button = current_push_button;

        // Update FPS counter and print FPS
        static int64_t previous_fps_time = 0;
        static int fps_counter = 0;
        fps_counter++;
        int64_t t = millis();
        if (t - previous_fps_time >= 1000) {
            printf("FPS: %d\n", fps_counter);
            fps_counter = 0;
            previous_fps_time = t;
        }
    }

    db_cluster_mngr_delete(db_cluster_mngr_ctx);
    gfx_delete(gfx_ctx);
    lcd_delete(lcd_ctx);
    pca9554_delete(pca9554_ctx);
    i2c_delete(i2c_ctx);
}

void core1_task(void *pvParameter) {
    I2S_CTX i2s_ctx;
    i2s_create(&i2s_ctx);

    while (true) {
        i2s_update(i2s_ctx);
    }

    i2s_delete(i2s_ctx);
}

void app_main(void) {
    xTaskCreatePinnedToCore(
        core1_task,
        "core1_task",
        4096,
        NULL,
        tskIDLE_PRIORITY,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        core0_task,
        "core0_task",
        4096,
        NULL,
        5,
        NULL,
        0);
}
