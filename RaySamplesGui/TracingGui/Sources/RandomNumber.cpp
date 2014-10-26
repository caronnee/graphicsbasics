#include "RandomNumber.h"


Vector4d Rng::GetVec3f()
{
	float a = GetFloat();
	float b = GetFloat();
	float c = GetFloat();

	return Vector4d(a, b, c, 0);
}

float Rng::GetFloat()
{
	return mDistFloat(mRng);
}

Rng::Rng(int aSeed /*= 1234*/) :
mRng(aSeed)
{

}
