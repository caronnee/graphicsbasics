#ifndef __MATERIAL__
#define __MATERIAL__

#include "Vector.h"

class Material
{
public:
	virtual Vector4d GetReflectance(const Vector4d & input, const Vector4d & output ) const =0;
};

enum MaterialType{
	MDiffuse
};

Material * CreateMaterial(MaterialType type, const Vector4d & parameters);

#endif