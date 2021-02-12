#include "pspiofilemgr.h"
#include "pspkerneltypes.h"
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include "psploadexec.h"
#include "pspthreadman.h"
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pspctrl.h>
#include <time.h>
#include "opcodes.h"
#include "gfx.h"
#include <pspiofilemgr_dirent.h>

PSP_MODULE_INFO("Chip-8", 0, 1, 1);

#define MEMORY_BEGIN 0X200
#define MEMORY_SIZE 0x1000

#define STACK_SIZE 0x10
#define V_SIZE 0x10

#define NPIXELS 0x800

uint8_t pixels[NPIXELS];

uint8_t memory[MEMORY_SIZE];
uint16_t pc;

uint16_t stack[STACK_SIZE];
uint8_t sp;

uint16_t I;
uint8_t v[V_SIZE];

uint8_t delay_timer;
uint8_t sound_timer;

uint8_t sprites[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void init() {

	GFX_init();
	pspDebugScreenInit();

	memset(pixels, 0x0, sizeof(pixels));

	memset(memory, 0x0, sizeof(memory));
	memcpy(memory, sprites, sizeof(sprites));
	pc = MEMORY_BEGIN;

	memset(stack, 0x0, sizeof(stack));
	sp = 0x0;

	I = 0x0;
	memset(v, 0x0, sizeof(v));

	delay_timer = 0x0;
	sound_timer = 0x0;

	srand(time(NULL));
}

int load_rom(char* rom) {

	FILE* fptr;

	if(!(fptr = fopen(rom, "rb")))
		return -1;

	fseek(fptr, 0x0, SEEK_END);
	int file_size = ftell(fptr);
	fseek(fptr, 0x0, SEEK_SET);

	fread(memory + MEMORY_BEGIN, file_size, 1, fptr);

	fclose(fptr);
	return 0;
}

void print_menu(char **list, uint8_t list_size, uint8_t option) {

	for (uint8_t i = 0; i < list_size; i++) {

		if(i == option)
			pspDebugScreenSetTextColor(0xFF00FFFF);
		else
			pspDebugScreenSetTextColor(0xFFFFFFFF);

		pspDebugScreenPrintf("%s\n", list[i]);
	}
}

int show_files(char *dir_name) {

	DIR *dirp;
	struct dirent *direntp;

	if(!(dirp = opendir(dir_name)))
		return -1;

	while ((direntp = readdir(dirp))) {
		pspDebugScreenPrintf("%s\n", direntp->d_name);
	}

	closedir(dirp);
	return 0;
}

int main() {

	init();
	load_rom("PONG");
	//uint16_t opcode;

	SceCtrlData pad;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	uint8_t option = 0;

	uint8_t list_size = 2;
	char **list = malloc(20 * list_size);
	list[0] = "Load rom";
	list[1] = "Exit";

	while (1) {
		pspDebugScreenSetXY(0, 0);
		sceCtrlReadBufferPositive(&pad, 1);

		print_menu(list, list_size, option);

		if(pad.Buttons & PSP_CTRL_UP)
			option -= 1;
		if(pad.Buttons & PSP_CTRL_DOWN)
			option += 1;

		if(pad.Buttons & PSP_CTRL_CIRCLE) {
			switch (option) {
				case 0:
					show_files(".");
					break;
				case 1:
					free(list);
					sceKernelExitGame();
					break;
			}
		}

		if(pad.Buttons > 0) {
			sceKernelDelayThread(1000 * 150);
			option %= list_size;
		}
	}
/*
	while (1) {

		opcode = (memory[pc] << 8) | memory[pc + 1];
		pc = (pc + 2) & 0xFFF;

		sceCtrlReadBufferPositive(&pad, 1);

		switch (opcode & 0xF000) {
			case 0x000:
				opcode_0000(opcode, pixels, NPIXELS, &pc, stack, &sp);
				break;
			case 0x1000:
				opcode_1nnn(opcode, &pc);
				break;
			case 0x2000:
				opcode_2nnn(opcode, &pc, stack, &sp);
				break;
			case 0x3000:
				opcode_3xkk(opcode, &pc, v);
				break;
			case 0x4000:
				opcode_4xkk(opcode, &pc, v);
				break;
			case 0x5000:
				opcode_5xy0(opcode, &pc, v);
				break;
			case 0x6000:
				opcode_6xkk(opcode, v);
				break;
			case 0x7000:
				opcode_7xkk(opcode, v);
				break;
			case 0x8000:
				opcode_8000(opcode, v);
				break;
			case 0x9000:
				opcode_9xy0(opcode, &pc, v);
				break;
			case 0xA000:
				opcode_Annn(opcode, &I);
				break;
			case 0xB000:
				opcode_Bnnn(opcode, &pc, v);
				break;
			case 0xC000:
				opcode_Cxkk(opcode, v);
				break;
			case 0xD000:
				opcode_Dxyn(opcode, pixels, memory, I, v);
				GFX_draw_rec(pixels, NPIXELS, 7, 7);
				GFX_swap_buffers();
				break;
			case 0xE000:
				opcode_E000(opcode, &pc, v, pad.Buttons);
				break;
			case 0xF000:
				opcode_F000(opcode, &delay_timer, &sound_timer, v, pad.Buttons, &I, memory);
				break;
			default:
				pspDebugScreenPrintf("Opcode(%x) not found.\n", opcode);
				sleep(2);
				sceKernelExitGame();
				break;
		}

		if(delay_timer != 0)
			delay_timer--;
		if(sound_timer != 0)
			sound_timer--;
	}
*/
	return 0;
}
