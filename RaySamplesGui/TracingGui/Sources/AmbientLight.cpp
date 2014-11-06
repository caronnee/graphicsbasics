#include "AmbientLight.h"
#include "mathutil.h"

AmbientLight::AmbientLight( const Vector4d & watts )
{
	SetPower(watts);
}

#include "RandomNumber.h"

Vector4d AmbientLight::SampleIllumination(const Vector4d & incoming,const Vector4d & norm, Vector4d& outputDirection)
{
	float cosA =norm.Dot(incoming);
	outputDirection = RandomHemisphereVector();
	outputDirection.Normalize();
	if ( cosA < 0 )
		return Vector4d(0,0,0,0);
	// sample hemisphere
	return _backColor * cosA * 4 * PI;
}

void AmbientLight::SetPower(const Vector4d & watts)
{
	_backColor = watts;
}
