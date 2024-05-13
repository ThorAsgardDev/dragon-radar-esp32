
#ifndef __I2S_H__
#define __I2S_H__


#include <inttypes.h>


typedef struct s_i2s_ctx *I2S_CTX;


void i2s_create(I2S_CTX *ctx);
void i2s_delete(I2S_CTX ctx);
void i2s_update(I2S_CTX ctx);
void i2s_play_sound_bip(void);
void i2s_play_sound_button(void);


#endif
