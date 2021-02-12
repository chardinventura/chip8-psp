#include <stdint.h>

// CLS
void opcode_0000(uint16_t opcode, uint8_t* pixels, uint16_t npixels, uint16_t* pc, uint16_t* stack, uint8_t* sp);

// JP addr
void opcode_1nnn(uint16_t opcode, uint16_t* pc);

// CALL addr
void opcode_2nnn(uint16_t opcode, uint16_t* pc, uint16_t* stack, uint8_t* sp);

// SE Vx, byte
void opcode_3xkk(uint16_t opcode, uint16_t* pc, uint8_t* v);

// SNE Vx, byte
void opcode_4xkk(uint16_t opcode, uint16_t* pc, uint8_t* v);

// SE Vx, Vy
void opcode_5xy0(uint16_t opcode, uint16_t* pc, uint8_t* v);

// LD Vx, byte
void opcode_6xkk(uint16_t opcode, uint8_t* v);

// ADD Vx, byte
void opcode_7xkk(uint16_t opcode, uint8_t* v);

// Opcodes of 8000
void opcode_8000(uint16_t opcode, uint8_t* v);

// SNE Vx, Vy
void opcode_9xy0(uint16_t opcode, uint16_t* pc, uint8_t* v);

// LD I, addr
void opcode_Annn(uint16_t opcode, uint16_t* I);

// JP V0, addr
void opcode_Bnnn(uint16_t opcode, uint16_t* pc, uint8_t* v);

// RND Vx, byte
void opcode_Cxkk(uint16_t opcode, uint8_t* v);

// DWR Vx, Vy, nibble
void opcode_Dxyn(uint16_t opcode, uint8_t* pixels, uint8_t* memory, uint16_t I, uint8_t* v);

// Opcodes of E000
void opcode_E000(uint16_t opcode, uint16_t *pc, uint8_t* v, uint16_t k);

// Opcodes of F000
void opcode_F000(uint16_t opcode, uint8_t* delay_timer,  uint8_t* sound_timer, uint8_t* v, uint16_t k, uint16_t* I, uint8_t* memory);
