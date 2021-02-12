#include <stdint.h>

void GFX_init();
void GFX_clear();
void GFX_swap_buffers();
void GFX_draw_rec(uint8_t* pixels, uint16_t npixels, uint16_t w, uint16_t h);
