#include <pspge.h>
#include <stdint.h>
#include "gfx.h"
#include "pspdebug.h"
#include "pspdisplay.h"
#include <time.h>
#include <psputils.h>

const uint16_t WIDTH = 480;
const uint16_t HEIGHT = 272;

const uint16_t BUFFER_WIDTH = 0x200;

uint32_t* display_buffer;
uint32_t* draw_buffer;

void GFX_init() {

	draw_buffer = sceGeEdramGetAddr();
	display_buffer = sceGeEdramGetAddr() + (BUFFER_WIDTH * HEIGHT * sizeof(uint32_t)); // BUFFER_WIDTH(power of 2) * HEIGHT * sizeof(uint32_t)

	sceDisplaySetMode(0, WIDTH, HEIGHT);
	sceDisplaySetFrameBuf(display_buffer, BUFFER_WIDTH, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
}

void GFX_clear() {

	for (uint32_t i = 0; i < (BUFFER_WIDTH * HEIGHT); i++)
		draw_buffer[i] = 0x00000000;
}

void GFX_swap_buffers() {

	uint32_t* temp = draw_buffer;
	draw_buffer = display_buffer;
	display_buffer = temp;

	sceKernelDcacheWritebackInvalidateAll();
	sceDisplaySetFrameBuf(display_buffer, BUFFER_WIDTH, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
}

void GFX_draw_rec(uint8_t* pixels, uint16_t npixels, uint16_t w, uint16_t h) {

	for (uint16_t i = 0; i < npixels; i++) {
		uint8_t x = (i % 0x40) + 2;
		uint8_t y = (i >> 0x6) + 3;

		uint32_t offset = x * w + (BUFFER_WIDTH * y * h);

		for (uint16_t x1 = 0; x1 < w; x1++) {
			for (uint16_t y1 = 0; y1 < h; y1++) {
				if(pixels[i])
					draw_buffer[BUFFER_WIDTH * y1 + x1 + offset] = 0xFFFFFFFF;
				else
					draw_buffer[BUFFER_WIDTH * y1 + x1 + offset] = 0x00000000;
			}
		}
	}
}
