#ifndef __RANDOM__
#define __RANDOM__

#include <random>
#include "Vector.h"

class Rng
{
public:
	Rng(int aSeed = 1234);

	  float GetFloat();

	  Vector4d GetVec3f();

private:

	std::mt19937_64 mRng;
	std::uniform_real_distribution<float> mDistFloat;
};

#endif