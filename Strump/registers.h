#pragma once

#ifndef REGISTERS_H
#define REGISTERS_H

#include "alias.h"
#include "SDL.h"

constexpr auto Z = 128;
constexpr auto N = 64;
constexpr auto H = 32;
constexpr auto C = 16;

// Registers
class Registers {
public:
	Registers() {
		sCounter = 0;
		PC = 0x100;
	}

	uint16_t SP;
	uint16_t PC;
	uint16_t rDiv;
	int32_t sCounter;

	union {
		struct {
			uint8_t f;
			uint8_t a;
		};
		uint16_t af;
	} AF;
	union {
		struct {
			uint8_t c;
			uint8_t b;
		};
		uint16_t bc;
	} BC;
	union {
		struct {
			uint8_t e;
			uint8_t d;
		};
		uint16_t de;
	} DE;
	union {
		struct {
			uint8_t l;
			uint8_t h;
		};
		uint16_t hl;
	} HL;
};

#endif