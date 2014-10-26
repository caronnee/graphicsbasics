#include "MaterialDiffuse.h"
#include "mathutil.h"

Vector4d MaterialDiffuse::GetDiffuse(const Vector4d & input, const Vector4d & output) const
{
	return _diffuseReflectance;
}

MaterialDiffuse::MaterialDiffuse(const Vector4d & diffuseReflectance, const Vector4d & emmisive) : base(emmisive), _diffuseReflectance (diffuseReflectance)
{
}

Vector4d MaterialDiffuse::EvalBrdf(const Vector4d & input, const Vector4d & output) const
{
	return GetDiffuse(input,output);
}

