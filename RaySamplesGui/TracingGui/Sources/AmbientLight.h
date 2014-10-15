#ifndef __AMBIENT_LIGHT__
#define __AMBIENT_LIGHT__

#include "Light.h"

class AmbientLight : public Light
{
	float _irradiance;
public:
	AmbientLight( const float & watts );
	virtual void SetPower( float watts );
	virtual float Irradiance ( const Vector4d & inputDirection, const Vector4d& outputDirection );
};
#endif