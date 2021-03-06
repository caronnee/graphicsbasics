#include "RandomNumber.h"

Rng james;

#define USE_SEQUENCE 0

#if USE_SEQUENCE
FILE * GSequence = NULL;
bool save = false;
#endif

#include "Debug.h"

float Rng::GetFloat()
{
#if USE_SEQUENCE
  float ret;
  if ( save )
	{
    ret = mDistFloat(mRng);
    fwrite(&ret, sizeof(ret),1, GSequence );
    return ret;
  }
  int len = fread(&ret, sizeof(ret),1,GSequence);

  if ( feof(GSequence) )
  {
    fseek(GSequence,0,SEEK_SET);
    len = fread(&ret, sizeof(ret),1,GSequence);
  }
  DoAssert(len == 1);
  return ret;
#else
  return mDistFloat(mRng);
#endif
}

#include "MathUtil.h"

float GetFloat()
{
	return james.GetFloat();
}

#include "Matrix.h"

Vector4d SampleUniform(const Vector4d & input,const Vector4d &normal, float &pdf)
{
  float e1 = james.GetFloat();
  float e2 = james.GetFloat()* 2 * PI;
  
  Vector4d ret = GetSpherePosition(e1,e2);
  
  ret[3] = 0;
  ret.Normalize();

  Matrix4d m;
  m.CreateFromZ(normal); 
  pdf = 0.5/PI;
  Vector4d ret2 = m.InSpace(ret);
  return ret2;
} 

#include "Debug.h"

Vector4d SampleHemisphereWeighted(int exponentRange /* 1 */)
{
  //create cos = e^(1/(n+1)
  float e1 = james.GetFloat();
  float e2 = james.GetFloat()* 2 * PI;
	Vector4d ret;
  float cosEl = pow(e1, 1.0f/(exponentRange+1));
  float sinEl = sqrt(1 - cosEl*cosEl);
  float cosAz = cos(e2);
  float sinAz = sin(e2);

  ret[0] = cosAz * sinEl;
  ret[1] = sinAz * sinEl;
  ret[2] = cosEl;
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
  ret[0] = cosA * sinB;
  ret[1] = sinA * sinB;
  ret[2] = cosB;
  ret[3] = 0;
	ret.Normalize();
	return ret;
}

#define SNAME "sequence.txt"

Rng::~Rng()
{
#if USE_SEQUENCE
  fclose(GSequence);
#endif
}

Rng::Rng(int aSeed /*= 1234*/) :
mRng(aSeed)
{
#if USE_SEQUENCE
  GSequence = fopen(SNAME,"rb");
  if ( GSequence == NULL )
  {
    GSequence = fopen(SNAME,"wb");
    save = true;
  }
#endif
}
