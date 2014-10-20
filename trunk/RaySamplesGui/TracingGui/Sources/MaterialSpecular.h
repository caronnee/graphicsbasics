#ifndef __MATERIAL_SPECULAR__
#define __MATERIAL_SPECULAR__

#include "MaterialDiffuse.h"

class MaterialSpecular : public MaterialDiffuse
{
	Vector4d _specularReflectance;
	float _phongCoef;
public:	
  MaterialSpecular(const Vector4d & diffuse, const Vector4d & specular, const float & phongCoef);

	Vector4d GetSpecular( const Vector4d & input, const Vector4d & output ) const;

	Vector4d GetTotalReflectance(const Vector4d & input, const Vector4d & output) const;
};
#endif