#include "Light.h"

void Light::SetGeometry(Geometry * geom)
{
	if (_geometry )
		delete _geometry;
	_geometry = geom;
}

Geometry * Light::GetGeometry() const
{
	return _geometry;
}
