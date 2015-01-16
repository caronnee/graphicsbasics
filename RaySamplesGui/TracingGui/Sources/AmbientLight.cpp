#include "AmbientLight.h"
#include "mathutil.h"
#include "Debug.h"

AmbientLight::AmbientLight( const Vector4d & watts )
{
	SetPower(watts);
}

#include "RandomNumber.h"

Vector4d AmbientLight::SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen )
{
	sampleLen = 1e36;
	sampledDir = SampleSphere();
	//lightVector = l;
	sampledDir.Normalize();
  return Radiance(sampledDir,section.nrm,sampleLen);
}

int AmbientLight::Type(void) const
{
	return -1;
}
void AmbientLight::SetPower(const Vector4d & watts)
{
	_backColor = watts;
}

bool AmbientLight::Intersect(const Ray &,Intersection &)
{
	return false;
}

Vector4d AmbientLight::Radiance(const Vector4d & lightVector, const Vector4d & nrm, const int & dummy)
{
  DoAssert( fabs(nrm.Size2() - 1 ) < EPSILON );
  DoAssert( fabs(lightVector.Size2() - 1 ) < EPSILON );
  float cosA = nrm.Dot(lightVector);
  if ( cosA < 0 )
    return Vector4d(0,0,0,0);
  DoAssert(cosA <=1.01);
  // sample hemisphere
  return _backColor * cosA; 
}

float AmbientLight::GetDirectionalPdf(const Vector4d & direction, const Vector4d& normal, const Vector4d& pos, const float & len)
{
  return 1.0f/(4*PI);
}
