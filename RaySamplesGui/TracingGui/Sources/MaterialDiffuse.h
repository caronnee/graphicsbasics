#ifndef __MATERIAL_DIFUSE__
#define __MATERIAL_DIFUSE__

#include "Material.h"

class MaterialDiffuse : public Material
{
	Vector4d _diffuseReflectance;
public:
	MaterialDiffuse(const Vector4d & diffuseReflectance);

	Vector4d GetDiffuse( const Vector4d & input, const Vector4d & output ) const;
	Vector4d GetReflectance(const Vector4d & input, const Vector4d & output ) const;
};

#endif