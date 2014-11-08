#include "AmbientLight.h"
#include "mathutil.h"
#include "Debug.h"
AmbientLight::AmbientLight( const Vector4d & watts )
{
	SetPower(watts);
}

#include "RandomNumber.h"

Vector4d AmbientLight::SampleIllumination(Intersection &section, Vector4d & lightVector, float & len )
{
	len = 1e36;
	lightVector = RandomHemisphereVector();
	//lightVector = l;
	lightVector.Normalize();
	float cosA = section.nrm.Dot(lightVector);
	if ( cosA < 0 )
		return Vector4d(0,0,0,0);
	DoAssert(cosA <=1.01);
	// sample hemisphere
	return _backColor * cosA * 4 * PI;
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
