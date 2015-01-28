#include "Vector.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

Vector4d::Vector4d(float a, float b, float c, float w)
{
	_val[0]=a;
	_val[1]=b;
	_val[2]=c;
	_val[3]=w;
}

Vector4d::Vector4d(const Vector4d& v)
{
	for ( int i = 0; i < 4; i++)
		_val[i] = v._val[i];
}

float Vector4d::X() const
{
	return _val[0];
}

float Vector4d::Y() const
{
	return _val[1];
}

float Vector4d::Z() const
{
	return _val[2];
}

float Vector4d::W() const
{
	return _val[3];
}
float& Vector4d::X()
{
	return _val[0];
}

float& Vector4d::Y()
{
	return _val[1];
}

float& Vector4d::Z()
{
	return _val[2];
}

float& Vector4d::W()
{
	return _val[3];
}

#include "Debug.h"

float Vector4d::Dot(const Vector4d& vec) const
{
//  DoAssert((Size2() -1) < EPSILON);
//  DoAssert((vec.Size2() -1) < EPSILON);
	return X()*vec.X() + Y()*vec.Y() + Z()*vec.Z() + W()*vec.W();
}

Vector4d Vector4d::Cross(const Vector4d& vec) const
{
	float x = Y() * vec.Z() - vec.Y()*Z(); 
	float y = -X() * vec.Z() + vec.X()*Z(); 
	float z = X() * vec.Y() - vec.X()*Y();
	return Vector4d(x,y,z,0);
}

Vector4d& Vector4d::operator=(const Vector4d & v)
{
	memcpy(_val,v._val,sizeof(_val));
	return *this;
}

float Vector4d::Size2() const
{
	return X() * X() + Y ()* Y() + Z()* Z();
}

float Vector4d::Size() const
{
	return sqrt(Size2());
}

Vector4d Vector4d::operator/(const float & oper)const
{
	Vector4d d(*this);
	d /= oper;
	return d;
}

void Vector4d::Normalize()
{
	float d = this->Size();
	X()/=d;
	Y()/=d;
	Z()/=d;
}

Vector4d Vector4d::operator-(const Vector4d & oper)const
{
	Vector4d d(*this);
	d -= oper;
	return d;
}
Vector4d Vector4d::operator-()const
{
	Vector4d d(*this);
	d *= -1;
	return d;
}

Vector4d& Vector4d::operator-=(const Vector4d & oper)
{
	for ( int i = 0; i < DIM; i++)
		_val[i]-=oper[i];
	return *this;
}

Vector4d Vector4d::operator+(const Vector4d & oper)const
{
	Vector4d d(*this);
	d+= oper;
	return d;
}
bool Vector4d::operator==(const Vector4d & oper)const
{
	Vector4d t = (*this).operator-(oper);
	float d = t.Size2();
	return d < 0.0001;
}

bool Vector4d::operator==(Vector4d& vec)
{
	for ( int i =0; i < 4; i++)
		if ( fabs(_val[i] - vec._val[i] ) > EPSILON )
			return false;
	return true;
}

float Vector4d::Max() const
{
	float m1 = _val[0] > _val[1]? _val[0] : _val[1];
	float m2 = m1 > _val[2]? m1 : _val[2];
	return m2;
}


float Vector4d::Min() const
{
	float m1 = _val[0] < _val[1]? _val[0] : _val[1];
	float m2 = m1 < _val[2]? m1 : _val[2];
	return m2;
}

Vector4d Vector4d::MultiplyPerElement(const Vector4d & oper)
{
	Vector4d ret;
	for ( int i = 0; i < DIM; i++)
	{
		ret._val[i]= _val[i] * oper._val[i];
	}
	return ret;
}

Vector4d& Vector4d::operator+=(const Vector4d & oper)
{
	for ( int i=0; i<DIM; i++)
		_val[i] += oper._val[i];
	return *this;
}
bool Vector4d::operator!=(Vector4d& vec)
{
	bool ret = (*this)==vec;
	return !ret;
}

Vector4d Vector4d::operator*(const float & oper)const
{
	Vector4d d(*this);
	d *= oper;
	return d;
}

Vector4d& Vector4d::operator*=(const float & oper)
{
	for ( int i =0; i < 4; i++)
		_val[i] *= oper;
	return *this;
}

Vector4d& Vector4d::operator/=(const float & oper)
{
	X() /= oper;
	Y() /= oper;
	Z() /= oper;
	W() /= oper;
	return *this;
}

void Vector4d::Zero()
{
  memset(_val,0,sizeof(_val));
}
