
#ifndef __GFX_H__
#define __GFX_H__


#include "lcd.h"

#include <inttypes.h>


#define RGB565(r, g, b) ((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | ((b) >> 3))

#define RGB565_R(c) (((c) >> 8) & 0xF8)
#define RGB565_G(c) (((c) >> 3) & 0xFC)
#define RGB565_B(c) (((c) << 3) & 0xF8)


typedef struct s_gfx_ctx *GFX_CTX;


void gfx_create(GFX_CTX *ctx, LCD_CTX lcd_ctx);
void gfx_delete(GFX_CTX ctx);
void gfx_fill_rect(GFX_CTX ctx, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
void gfx_draw_image(GFX_CTX ctx, int x, int y, const uint8_t *src_data, int w, int h);
void gfx_draw_h_line(GFX_CTX ctx, int y, int thickness);
void gfx_draw_v_line(GFX_CTX ctx, int x, int thickness);
int gfx_get_width(GFX_CTX ctx);
int gfx_get_height(GFX_CTX ctx);
void gfx_update(GFX_CTX ctx);


#endif
