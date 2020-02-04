#include "Card.h"

Card::Card() {
	income = 0;
	power = 0;
	resistance = 0;
}
Card::~Card() {}

int Card::Income() { return income; }
int Card::Power() { return power; }
int Card::Resistance() { return resistance; }