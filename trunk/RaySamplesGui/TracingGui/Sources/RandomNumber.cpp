#include "RandomNumber.h"

Rng james;

float Rng::GetFloat()
{
	return mDistFloat(mRng);
}

#include "MathUtil.h"

float GetFloat()
{
	return james.GetFloat();
}

#include "Debug.h"

Vector4d SampleHemisphere()
{
	float azimuthAngle = james.GetFloat() * 2 * PI;
	float elevationAngle = james.GetFloat() * PI;
	float cosA = cos(elevationAngle);
	float cosB = cos(azimuthAngle);
	float sinA = sin(elevationAngle);
	float sinB = sin(azimuthAngle);
	Vector4d ret;
	ret[0] = cosA* cosB;
	ret[1] = sinA;
  DoAssert(sinA > 0);
	ret[2] = cosA * sinB;
	ret[3] = 0;
	ret.Normalize();
	return ret;
}

Vector4d SampleSphere()
{
	/* Vector4d ret(0,0,0,0);
	do
	{
		ret[0] = james.GetFloat()-0.5f;
		ret[1] = james.GetFloat()-0.5f;
		ret[2] = james.GetFloat()-0.5f;
	}while (ret.Size() > 0.5f);
	ret.Normalize(); */
	float alpha = james.GetFloat() *2*PI;
	float beta = james.GetFloat() *2*PI;
	float cosA = cos(alpha);
	float cosB = cos(beta);
	float sinA = sin(alpha);
	float sinB = sin(beta);
	Vector4d ret;
	ret[0] = cosA* cosB;
	ret[1] = sinA;
	ret[2] = cosA * sinB;
	ret[3] = 0;
	ret.Normalize();
	return ret;
}

Rng::Rng(int aSeed /*= 1234*/) :
mRng(aSeed)
{

}
