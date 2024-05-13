
#include "gfx.h"

#include <string.h>


struct s_gfx_ctx {
    LCD_CTX lcd_ctx;
    int width;
    int height;
};


void gfx_create(GFX_CTX *gfx_ctx, LCD_CTX lcd_ctx) {
    struct s_gfx_ctx *ctx = (struct s_gfx_ctx *)malloc(sizeof(struct s_gfx_ctx));

    *gfx_ctx = ctx;

    memset(ctx, 0, sizeof(struct s_gfx_ctx));

    ctx->lcd_ctx = lcd_ctx;
    ctx->width = lcd_get_width(lcd_ctx);
    ctx->height = lcd_get_height(lcd_ctx);
}

void gfx_delete(GFX_CTX ctx) {
    free(ctx);
}

void gfx_fill_rect(GFX_CTX ctx, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) {
    uint16_t *dst = lcd_get_frame_buffer(ctx->lcd_ctx) + y * ctx->width + x;
    uint16_t *first_line = dst;

    uint32_t iw = w;

    if (h--) {
        while (iw--) *dst++ = color;
    }

    dst = first_line;
    while (h--) {
        dst += ctx->width;
        memcpy(dst, first_line, w << 1);
    }
}

void gfx_draw_image(GFX_CTX ctx, int x, int y, const uint8_t *src_data, int w, int h) {
    uint16_t *dst = lcd_get_frame_buffer(ctx->lcd_ctx) + y * ctx->width + x;
    uint8_t *src = (uint8_t *)src_data;
    int yy = h;
    while (yy--) {
        int xx = w;
        while (xx--) {
            uint8_t a = *src++;
            switch (a) {
                case 0:
                    dst++;
                    src += 3;
                    break;
                case 255:
                    *dst++ = RGB565(*src, *(src + 1), *(src + 2));
                    src += 3;
                    break;
                default:
                    uint8_t dst_r = RGB565_R(*dst);
                    uint8_t dst_g = RGB565_G(*dst);
                    uint8_t dst_b = RGB565_B(*dst);
                    *dst++ = RGB565(
                        ((a * ((*src) - dst_r)) >> 8) + dst_r,
                        ((a * ((*(src + 1)) - dst_g)) >> 8) + dst_g,
                        ((a * ((*(src + 2)) - dst_b)) >> 8) + dst_b
                    );
                    src += 3;
            }
        }
        dst += ctx->width - w;
    }
}

void gfx_draw_h_line(GFX_CTX ctx, int y, int thickness) {
    uint16_t *dst = lcd_get_frame_buffer(ctx->lcd_ctx) + (y - (thickness >> 1)) * ctx->width;
    while (thickness--) {
        memset(dst, 0, ctx->width << 1);
        dst += ctx->width;
    }
}

void gfx_draw_v_line(GFX_CTX ctx, int x, int thickness) {
    uint16_t *dst = lcd_get_frame_buffer(ctx->lcd_ctx) + x - (thickness >> 1);
    int length = thickness << 1;
    int y = ctx->height;
    while (y--) {
        memset(dst, 0, length);
        dst += ctx->width;
    }
}

int gfx_get_width(GFX_CTX ctx) {
    return ctx->width;
}

int gfx_get_height(GFX_CTX ctx) {
    return ctx->height;
}

void gfx_update(GFX_CTX ctx) {
    lcd_update(ctx->lcd_ctx);
}
