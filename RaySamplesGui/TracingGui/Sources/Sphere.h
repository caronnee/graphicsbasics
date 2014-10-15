#ifndef __SPHERE_MODEL__
#define __SPHERE_MODEL__

#include "Geometry.h"

class Sphere : public Geometry
{
public:
	Sphere( float radius);
	virtual bool Intersect( const Ray & ray, Intersection & sect );
	int Type()const;
};

#endif