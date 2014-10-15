#include "MaterialSpecular.h"
#include "mathutil.h"

MaterialSpecular::MaterialSpecular(float diffuse, float specular, float phongCoef) : MaterialDiffuse(diffuse),
	_specularReflectance(specular),
	_phongCoef(phongCoef)
{

}


Vector4d MaterialSpecular::GetSpecular(const Vector4d & input, const Vector4d & output) const
{
	float cosA = input.Dot(output);
	float cosAn = pow(cosA,_phongCoef);
	Vector4d v = _specularReflectance * (_phongCoef + 2.0f) * cosAn /( 2 * PI);
	return v;
}

Vector4d MaterialSpecular::GetReflectance(const Vector4d & input, const Vector4d & output) const
{
	return GetDiffuse(input,output) + GetSpecular(input,output);
}
