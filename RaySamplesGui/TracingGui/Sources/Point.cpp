#include "Point.h"

bool PointObject::Intersect(const Ray & ray, Intersection & sect)
{
	// rayOrigin + q*rayDirection == point
	Vector4d worldPos = ModelToWorld(Vector4d(0,0,0,1));
	Vector4d correctDir = worldPos - ray.origin;
	correctDir.Normalize();
	Vector4d correctDir2 = ray.direction;
	float test = correctDir2.Max();
	correctDir2.Normalize();
	Vector4d diff = correctDir - correctDir2;
	if ( diff.Size2() < EPSILON )
	{
		sect.model = this;
		sect.positionModel = Vector4d(0,0,0,1);
		sect.t = correctDir2.Max()/ray.direction.Max();
		return true;
	}
	return false;
}

int PointObject::Type() const
{
	return TypePoint;
}
