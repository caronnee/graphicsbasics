#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Point.h"

class PointLight : public Light
{
	float _watts;
public:
	PointLight( float watts );
	virtual void SetPower( float watts );
	virtual float Irradiance( const Vector4d & inputDirection, const Vector4d& outputDirection );
};
#endif
