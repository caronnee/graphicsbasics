#ifndef __PLANE__
#define __PLANE__

#include "Geometry.h"

class Plane : public Geometry
{
	float _plusW, _plusH;
public:
	Plane(const float & w, const float & h);
	virtual bool Intersect( const Ray & ray, Intersection & sect );
};

#endif