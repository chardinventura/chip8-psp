#include "opcodes.h"
#include <time.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <string.h>
#include <stdlib.h>
#include <pspctrl.h>

uint32_t keys[] = {
	PSP_CTRL_VOLUP, PSP_CTRL_UP, PSP_CTRL_LEFT, PSP_CTRL_RIGHT,
	PSP_CTRL_DOWN, PSP_CTRL_VOLDOWN, PSP_CTRL_SQUARE, PSP_CTRL_CIRCLE,
	PSP_CTRL_LTRIGGER, PSP_CTRL_RTRIGGER, PSP_CTRL_SELECT, PSP_CTRL_START,
	PSP_CTRL_TRIANGLE, PSP_CTRL_CROSS, PSP_CTRL_HOLD, PSP_CTRL_SCREEN
};

void opcode_0000(uint16_t opcode, uint8_t* pixels, uint16_t npixels, uint16_t* pc, uint16_t* stack, uint8_t* sp) {
	switch (opcode & 0xFF) {
		// CLS
		case 0xE0:
			memset(pixels, 0x0, npixels);
			break;
		// RET
		case 0xEE:
			if(*sp > 0)
				*pc = stack[--(*sp)];
			break;
		default:
			pspDebugScreenPrintf("Opcode(%x) not found.\n", opcode);
			sceKernelDelayThread(1000 * 5000);
			sceKernelExitGame();
			break;
	}
}

// JP addr
void opcode_1nnn(uint16_t opcode, uint16_t* pc) {
	*pc = opcode & 0xFFF;
}

// CALL addr
void opcode_2nnn(uint16_t opcode, uint16_t* pc, uint16_t* stack, uint8_t* sp) {
	if(*sp < 0xF)
		stack[(*sp)++] = *pc;
	*pc = opcode & 0xFFF;
}

// SE Vx, byte
void opcode_3xkk(uint16_t opcode, uint16_t* pc, uint8_t* v) {
	if(v[(opcode >> 8) & 0xF] == (opcode & 0xFF))
		*pc += 2;
}

// SNE Vx, byte
void opcode_4xkk(uint16_t opcode, uint16_t* pc, uint8_t* v) {
	if(v[(opcode >> 8) & 0xF] != (opcode & 0xFF))
		*pc += 2;
}

// SE Vx, Vy
void opcode_5xy0(uint16_t opcode, uint16_t* pc, uint8_t* v) {
	if(v[(opcode >> 8) & 0xF] == v[(opcode >> 4) & 0xF])
		*pc += 2;
}

// LD Vx, byte
void opcode_6xkk(uint16_t opcode, uint8_t* v) {
	v[(opcode >> 8) & 0xF] = (opcode & 0xFF);
}

// ADD Vx, byte
void opcode_7xkk(uint16_t opcode, uint8_t* v) {
	v[(opcode >> 8) & 0xF] = (v[(opcode >> 8) & 0xF] + (opcode & 0xFF)) & 0xFF;
}

// Opcodes of 8000
void opcode_8000(uint16_t opcode, uint8_t* v) {
	uint8_t x = (opcode >> 8) & 0xF;
	uint8_t y = (opcode >> 4) & 0xF;
	switch (opcode & 0xF) {
		// LD Vx, Vy
		case 0x0:
			v[x] = v[y];
			break;
		// OR Vx, Vy
		case 0x1:
			v[x] |= v[y];
			break;
		// AND Vx, Vy
		case 0x2:
			v[x] &= v[y];
			break;
		// XOR Vx, Vy
		case 0x3:
			v[x] ^= v[y];
			break;
		// ADD Vx, Vy
		case 0x4:
			v[0xF] = (v[x] + v[y]) > 0xFF;
			v[x] = (v[x] + v[y]) & 0xFF;
			break;
		// SUB Vx, Vy
		case 0x5:
			v[0xF] = (v[x] > v[y]);
			v[x] = (v[x] - v[y]) & 0xFF;
			break;
		// SHR Vx {, Vy}
		case 0x6:
			v[0xF] = v[x] & 0x1;
			v[x] = (v[x] >> 2) & 0xFF;
			break;
		// SUBN Vx, Vy
		case 0x7:
			v[0xF] = (v[y] > v[x]);
			v[x] = (v[y] - v[x]) & 0xFF;
			break;
		// SHL Vx {, Vy}
		case 0xE:
			v[0xF] = (v[x] >> 7) & 0x1;
			v[x] = (v[x] << 2) & 0xFF;
			break;
		default:
			pspDebugScreenPrintf("Opcode(%x) not found.\n", opcode);
			sceKernelDelayThread(1000 * 5000);
			sceKernelExitGame();
			break;
	}
}

// SNE Vx, Vy
void opcode_9xy0(uint16_t opcode, uint16_t* pc, uint8_t* v) {
	if(v[(opcode >> 8) & 0xF] != v[(opcode >> 4) & 0xF])
		*pc += 2;
}

// LD I, addr
void opcode_Annn(uint16_t opcode, uint16_t* I) {
	*I = opcode & 0xFFF;
}

// JP V0, addr
void opcode_Bnnn(uint16_t opcode, uint16_t* pc, uint8_t* v) {
	*pc = (v[0] + (opcode & 0xFFF)) & 0xFFF;
}

// RND Vx, byte
void opcode_Cxkk(uint16_t opcode, uint8_t* v) {
	v[(opcode >> 8) & 0xF] = rand() & (opcode & 0xFF);
}

// DWR Vx, Vy, nibble
void opcode_Dxyn(uint16_t opcode, uint8_t* pixels, uint8_t* memory, uint16_t I, uint8_t* v) {

	v[0xF] = 0;
	for (uint8_t i = 0; i < (opcode & 0xF); i++) {
		uint8_t sprite = memory[I + i];
		for (uint8_t j = 0; j < 8; j++) {
			uint8_t px = (v[(opcode >> 8) & 0xF] + j) & 0x3F;
			uint8_t py = (v[(opcode >> 4) & 0xF] + i) & 0x1F;
			pixels[0x40 * py + px] ^= (sprite >> (7-j)) & 0x1;
			v[0xF] = pixels[0x40 * py + px];
		}
	}
}

// Opcodes of E000
void opcode_E000(uint16_t opcode, uint16_t *pc, uint8_t* v, uint32_t k) {
	switch (opcode & 0xFF) {
		// SKP Vx
		case 0x9E:
			if(keys[v[(opcode >> 8) & 0xF]] & k)
				*pc += 2;
			break;
		// SKNP Vx
		case 0xA1:
			if(keys[v[(opcode >> 8) & 0xF]] ^ k)
				*pc += 2;
			break;
		default:
			pspDebugScreenPrintf("Opcode(%x) not found.\n", opcode);
			sceKernelDelayThread(1000 * 5000);
			sceKernelExitGame();
			break;
	}
}

// Opcodes of F000
void opcode_F000(uint16_t opcode, uint8_t* delay_timer, uint8_t* sound_timer,
	   	uint8_t* v, uint16_t k, uint16_t* I, uint8_t* memory) {

	uint8_t x = (opcode >> 8) & 0xF;

	switch (opcode & 0xFF) {
		// LD Vx, DT
		case 0x07:
			v[x] = *delay_timer;
			break;
		// LD Vx, K
		case 0x0A:
			// TODO
			break;
		// LD DT, Vx
		case 0x15:
			*delay_timer = v[x];
			break;
		// LD ST, Vx
		case 0x18:
			*sound_timer = v[x];
			break;
		// ADD I, Vx
		case 0x1E:
			*I = (*I + v[x]) & 0xFFF;
			break;
		// LD F, Vx
		case 0x29:
			*I = ((v[x] & 0xF) * 5);
			break;
		// LD B, Vx
		case 0x33:
			memory[*I] = v[x] / 100;
			memory[*I + 1] = (v[x] / 10) % 0xA;
			memory[*I + 2] = v[x] % 0xA;
			break;
		// LD [I], Vx
		case 0x55:
			for (uint8_t i = 0; i <= x; i++)
				memory[*I + i] = v[i];
			break;
		// LD Vx, [I]
		case 0x65:
			for (uint8_t i = 0; i <= x; i++)
				v[i] = memory[*I + i];
			break;
		default:
			pspDebugScreenPrintf("Opcode(%x) not found.\n", opcode);
			sceKernelDelayThread(1000 * 5000);
			sceKernelExitGame();
			break;
	}
}
