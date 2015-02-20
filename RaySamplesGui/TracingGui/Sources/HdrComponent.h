#ifndef __HDRCOMPONENT__
#define __HDRCOMPONENT__

#include "Vector.h"

class HDRComponent: public Vector4d
{
  typedef Vector4d base;
  int _weight;
public:
	HDRComponent();
	HDRComponent( const HDRComponent & comp );
	void Normalize();
  HDRComponent & Add(const Vector4d & val);
  void SetUnit();
  HDRComponent & operator =(const Vector4d & oper)
  {
    base::operator=(oper);
    return *this;
  }
  void Clear();
};
#endif