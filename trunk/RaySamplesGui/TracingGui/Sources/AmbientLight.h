#ifndef __AMBIENT_LIGHT__
#define __AMBIENT_LIGHT__

#include "Vector.h"
#include "Geometry.h"

class AmbientLight : public Geometry
{
	Vector4d _backColor;
public:
	AmbientLight( const Vector4d & watts );
	void SetPower( const Vector4d & watts );
	bool Intersect(const Ray &,Intersection &);
	Vector4d SampleIllumination(Intersection &section, Vector4d & lightVector, float & len );
	int Type(void) const;
};
#endif