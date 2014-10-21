#include "PointLight.h"
#include "MathUtil.h"

PointLight::PointLight( float watts )
{
	SetPower(watts);
}

void PointLight::SetPower(float watts)
{
	_watts = watts;
}

float PointLight::Irradiance(const Vector4d & inputDirection, const Vector4d& outputDirection)
{
	return _watts/(4 * PI);
}