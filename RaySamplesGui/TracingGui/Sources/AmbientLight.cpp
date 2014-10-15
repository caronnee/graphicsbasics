#include "AmbientLight.h"

AmbientLight::AmbientLight( const float & watts )
{
	SetPower(watts);
}

float AmbientLight::Irradiance(const Vector4d & inputDirection, const Vector4d& outputDirection)
{
	return _irradiance;
}

void AmbientLight::SetPower(float watts)
{
	_irradiance = watts;
}
