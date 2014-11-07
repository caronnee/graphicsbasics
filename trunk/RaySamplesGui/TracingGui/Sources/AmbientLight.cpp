#include "AmbientLight.h"
#include "mathutil.h"
#include "Debug.h"
AmbientLight::AmbientLight( const Vector4d & watts )
{
	SetPower(watts);
}

#include "RandomNumber.h"

Vector4d AmbientLight::SampleIllumination(const Vector4d & incoming,const Vector4d & norm, Vector4d& lightVector)
{
	lightVector = RandomHemisphereVector();
	//lightVector = l;
	lightVector.Normalize();
	float cosA = norm.Dot(lightVector)/(norm.Size()*lightVector.Size());
	if ( cosA < 0 )
		return Vector4d(0,0,0,0);
	DoAssert(cosA <=1.01);
	// sample hemisphere
	return _backColor * cosA * 4 * PI;
}

void AmbientLight::SetPower(const Vector4d & watts)
{
	_backColor = watts;
}
