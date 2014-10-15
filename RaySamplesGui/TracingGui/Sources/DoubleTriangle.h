#ifndef ___DOUBLE_TRIANGLE__
#define ___DOUBLE_TRIANGLE__

#include "Geometry.h"

class DoubleTriangle :  public Geometry
{
	Vector4d _points[3];
	Vector4d _edges[2];
public:
	DoubleTriangle( Vector4d * trianglePoints );
	bool Intersect( const Ray & ray, Intersection & sect );
	void SetPoints(Vector4d * trianglePoints);
	void SetProperty(PropertyType type, void * values);
	void * GetProperty(PropertyType type);
	int Type() const;
};

#endif
