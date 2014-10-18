#ifndef __AMBIENT_LIGHT__
#define __AMBIENT_LIGHT__

#include "Vector.h"

class AmbientLight
{
	Vector4d _irradiance;
public:
	AmbientLight( const Vector4d & watts );
	void SetPower( const Vector4d & watts );
	Vector4d Irradiance ( const Vector4d & inputDirection, const Vector4d& outputDirection );
};
#endif