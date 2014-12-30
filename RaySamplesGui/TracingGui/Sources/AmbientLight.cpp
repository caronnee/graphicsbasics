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
  return Illumination(sampledDir,section.nrm,sampleLen,1.0/(4*PI));
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

Vector4d AmbientLight::Illumination(const Vector4d & lightVector, const Vector4d & nrm, const int & dummy, float pdf)
{
  DoAssert( fabs(lightVector.Size2() - 1 ) < EPSILON );
  float cosA = nrm.Dot(lightVector);
  if ( cosA < 0 )
    return Vector4d(0,0,0,0);
  DoAssert(cosA <=1.01);
  // sample hemisphere
  return _backColor * cosA / pdf; 
}

float AmbientLight::GetPdf(const Vector4d & direction)
{
  return 1/ 4 *PI;
}
