#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <ctime>
#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <cstdint>
#include <algorithm>

class Random
{
	public:
		Random();
		~Random();
		float randomNumber(float min, float max);
		float uniformFloat();
	private:

};
