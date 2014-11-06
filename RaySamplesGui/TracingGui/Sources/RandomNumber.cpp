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
	int i;
	for ( i =0; i < 3; i++)
	{
		ret[0] = james.GetFloat();
	}
	ret[i] = 0;
	return ret;
}

Rng::Rng(int aSeed /*= 1234*/) :
mRng(aSeed)
{

}
