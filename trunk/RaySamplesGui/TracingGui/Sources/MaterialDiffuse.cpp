#include "MaterialDiffuse.h"
#include "mathutil.h"

Vector4d MaterialDiffuse::GetDiffuse(const Vector4d & input, const Vector4d & output) const
{
	return _diffuseReflectance/PI;
}

MaterialDiffuse::MaterialDiffuse(const Vector4d & diffuseReflectance) : _diffuseReflectance (diffuseReflectance)
{

}

Vector4d MaterialDiffuse::GetReflectance(const Vector4d & input, const Vector4d & output) const
{
	return GetDiffuse(input,output);
}

