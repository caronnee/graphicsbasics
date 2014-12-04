#include "HdrComponent.h"

#include <stdio.h>
#include <cmath>

#define max(a,b) ( a>b? a:b)

void HDRComponent::Normalize()
{
	float v = max(_val[0], max(_val[1], _val[2]));

	if(v >= 1e-32f)
	{
		int e;
		v = float(frexp(v, &e) * 1.0f / v);
		_val[0] = (_val[0] * v);
		_val[1] = (_val[1] * v);
		_val[2] = (_val[2] * v);
		_val[3] = e; 
	}
}

HDRComponent::HDRComponent()
{
  _weight = 0;
	for ( int i=0; i <4; i++) _val[i] = 0;
}
HDRComponent::HDRComponent( const HDRComponent & comp )
{
  _weight = comp._weight;
	Vector4d::operator=(comp);
}

HDRComponent & HDRComponent::Add(const Vector4d & val)
{
  base::operator+=(val);
  _weight+=1;
  return *this;
}

void HDRComponent::SetUnit()
{
  if ( _weight == 0)
    return;// nothing was there
  base::operator/=(_weight);
  _weight = 1;
}
