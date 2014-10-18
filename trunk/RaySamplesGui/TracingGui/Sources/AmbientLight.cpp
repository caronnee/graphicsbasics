#include "AmbientLight.h"

AmbientLight::AmbientLight( const Vector4d & watts )
{
	SetPower(watts);
}

Vector4d AmbientLight::Irradiance(const Vector4d & inputDirection, const Vector4d& outputDirection)
{
	return _irradiance;
}

void AmbientLight::SetPower(const Vector4d & watts)
{
	_irradiance = watts;
}
