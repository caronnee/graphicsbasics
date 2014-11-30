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

Vector4d SampleHemisphere(int exponentRange /* 1 */)
{
  //create cos = e^(1/(n+1)
  float e1 = james.GetFloat();
  float e2 = james.GetFloat();
	Vector4d ret;
  float aizmuth = 2* e1 * PI;
  float cosEl = pow(e1,exponentRange+1);
  float sinEl = sqrt(1 - cosEl*cosEl);
  float cosAz = cos(e2);
  float sinAz = sin(e2);

  ret[0] = cosAz * sinEl;
  ret[2] = cosEl;
  ret[1] = cosAz * cosEl;
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
