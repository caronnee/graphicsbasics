#include "Point.h"

bool PointObject::Intersect(const Ray & ray, Intersection & sect)
{
	Vector4d originToModel = WorldToModel( ray.origin );
	if ( originToModel.Size2() == 0.0f )
		return true; // it is at the same point
	Vector4d v2 = Vector4d(0,0,0,1) - ray.origin;
	float coef ;
	if ( v2.Max() != 0)
		coef = ray.direction.Max() / v2.Max();
	else
		coef = ray.direction.Min() / v2.Min();
	Vector4d calc = ray.origin + ray.direction * coef;
	sect.model = this;
	sect.positionModel = calc;
	sect.t = coef;
	return true;
}

int PointObject::Type() const
{
	return TypePoint;
}
