#ifndef __LIGHT__
#define __LIGHT__

#include "Vector.h"
#include "Geometry.h"

class Light
{
	Geometry * _geometry;
public:
	virtual void SetPower( float watts ) = 0;
	virtual float Irradiance ( const Vector4d & inputDirection, const Vector4d& outputDirection ) = 0;
	Geometry * GetGeometry()const;
	void SetGeometry(Geometry * geom);
};
#endif