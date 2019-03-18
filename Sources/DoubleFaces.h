#include "Geometry.h"

class DoubleFaces //:public Geometry
{
public:
	bool Intersect( const Ray & ray, Intersection & sect );
};