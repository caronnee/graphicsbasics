#include "Plane.h"
#include "Debug.h"

Plane::Plane(const float & w, const float & h)
{
	// around center
	_plusH = h/2;
	_plusW = w/2;
}

bool Plane::Intersect(const Ray & ray, Intersection & sect)
{
	Vector4d modelOrigin (0,0,0,1);
	Vector4d rayOrigin = WorldToModel(ray.origin);
	Vector4d rayDirect = WorldToModel(ray.direction);
	float coef = 0;
	if ( rayOrigin.Z() != 0 )
	{
		if (rayDirect.Z() == 0)
			return false;
		coef = -rayOrigin.Z()/rayDirect.Z();
	}
	if ( coef<= MIN_DIST)
		return false;
	Vector4d localCoord = rayOrigin + rayDirect * coef;
	if ( fabs(localCoord.X()) > _plusW )
		return false;
	if ( fabs(localCoord.Y()) > _plusH)
		return false;
	DoAssert( fabs(localCoord.Z()) < EPSILON );
	sect.model = this;
	sect.worldPosition = ray.origin + ray.direction * coef;
	sect.t = coef;
	sect.nrm = ModelToWorld(Vector4d(0,0,1,0));
	return true;
}
