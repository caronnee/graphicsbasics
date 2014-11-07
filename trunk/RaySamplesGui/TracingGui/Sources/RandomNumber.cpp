#include "RandomNumber.h"

Rng james;

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

Vector4d RandomHemisphereVector()
{
	Vector4d ret;
	// sample from whole hemisphere
	ret[0] = james.GetFloat() - 0.5f;
	ret[1] = james.GetFloat() - 0.5f;
	ret[2] = james.GetFloat() - 0.5f;
	ret[3] = 0;
	ret.Normalize();
	return ret;
}

Rng::Rng(int aSeed /*= 1234*/) :
mRng(aSeed)
{

}
