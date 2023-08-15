#pragma once

#include "alias.h"
#include <stdint.h>

typedef struct _tile {
	uint8_t data[16];
	uint16_t address;
} tile;