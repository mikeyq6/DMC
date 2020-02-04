#pragma once
class Card
{
public:
	Card();
	~Card();

	int Income();
	int Power();
	int Resistance();

private:
	int income;
	int power;
	int resistance;
};

