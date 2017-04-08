#include "Random.h"

Random::Random()
{
	
}
Random::~Random()
{
	
}
/*Uniformly distributed random float*/
float Random::uniformFloat()
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(1.0,10.0);
	float number;
	for(int i = 0; i < 20; i++)
	{
		number = distribution(generator);
	//	printf("Random Number: %f\n", number);
	}
	return number;
}
/*Given a min and max range give a random number back*/
float Random::randomNumber(float min, float max)
{
	srand((unsigned)time(0));
	float range = max - min;
	
	float number;
	for(int i = 0; i < 20; i++)
	{
		number = min + ((range * rand()) / (RAND_MAX + 1.0)) ;
	//	printf("Random Number: %f\n", number);
	}
	return number;
}
/*
int main()
{
	float min = 0.0;
	float max = 1.0;
	//uniformFloat();
	randomTest(min, max);
 return 0;
}
*/
