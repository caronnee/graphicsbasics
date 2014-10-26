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
		sect.worldPosition = worldPos;
		sect.t = correctDir2.Max()/ray.direction.Max();
		sect.nrm = Vector4d(0,0,0,0); // point object does not have normal
		return true;
	}
	return false;
}

int PointObject::Type() const
{
	return TypePoint;
}

Vector4d PointObject::SampleIllumination(Intersection &section)
{
	throw "Not implemented yet";
}
