#ifndef __RANDOM__
#define __RANDOM__

#include <random>
#include "Vector.h"

class Rng
{
public:
	Rng(int aSeed = 1234);

	  float GetFloat();
    ~Rng();
private:

	std::mt19937_64 mRng;
	std::uniform_real_distribution<float> mDistFloat;
};

Vector4d SampleSphere();
Vector4d SampleHemisphereWeighted(int exponentRange = 1);
float GetFloat();
Vector4d SampleUniform(const Vector4d & input,const Vector4d &normal, float &pdf);
#endif