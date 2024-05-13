
#ifndef __LCD_H__
#define __LCD_H__


#include "pca9554.h"

#include <inttypes.h>


typedef struct s_lcd_ctx *LCD_CTX;


void lcd_create(LCD_CTX *ctx, PCA9554_CTX pca9554_ctx);
void lcd_delete(LCD_CTX ctx);
uint16_t *lcd_get_frame_buffer(LCD_CTX ctx);
int lcd_get_width(LCD_CTX ctx);
int lcd_get_height(LCD_CTX ctx);
void lcd_update(LCD_CTX ctx);


#endif
