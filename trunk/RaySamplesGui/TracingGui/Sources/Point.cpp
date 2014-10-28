#include "Point.h"
#include "Debug.h"

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
	if ( diff.Size2() < 1e-08 )
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

Vector4d PointObject::SampleIllumination(Intersection &section, Vector4d & sampledDir, float & sampleLen)
{
	DoAssert(GetMaterial()->IsLight());
	Vector4d & intensity = GetMaterial()->Emmisive();
	Vector4d norm = section.nrm;
	norm.Normalize();
	sampledDir = section.worldPosition - ModelToWorld(Vector4d(0,0,0,1));
	float r2 = sampledDir.Size2();
	sampledDir.Normalize();
	float cosa = norm.Dot(-sampledDir);
	
	if ( cosa < 0 )
		return Vector4d(0,0,0,0);
	return intensity*cosa/r2;
}
