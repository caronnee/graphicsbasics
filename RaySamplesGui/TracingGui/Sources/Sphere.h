#ifndef __SPHERE_MODEL__
#define __SPHERE_MODEL__

#include "Geometry.h"

class Sphere : public Geometry
{
	typedef Geometry base;
public:
	Sphere( float radius);
	virtual bool Intersect( const Ray & ray, Intersection & sect );
	Vector4d SampleIllumination(const Vector4d & worlsposition);
	int Type()const;
	void * GetProperty(PropertyType type);
	void SetProperty(PropertyType type, void * value);
	virtual Vector4d SampleIllumination(Intersection &section );
	float _radius;
	float _radius2;
};

#endif