#ifndef __MATERIAL__
#define __MATERIAL__

#include "Vector.h"

class Material
{
  Vector4d _emmisive;
public:
  Material(Vector4d e);
  bool IsLight();
  Vector4d Emmisive() { return _emmisive; }
	virtual Vector4d GetTotalReflectance(const Vector4d & input, const Vector4d & output ) const =0;
};

enum MaterialType{
	MDiffuse
};

Material * CreateMaterial(MaterialType type, const Vector4d & parameters, const Vector4d & emmisive = Vector4d(0,0,0,0));

#endif