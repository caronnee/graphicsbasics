#ifndef __VECTOR__
#define __VECTOR__

#include <io.h>
#include <float.h>
#include <math.h>

#define DIM 4
#define EPSILON 1e-4

class Vector4d
{
protected:
	// last one is weight and should always be one. Never expected to be 0!
	float _val[4];
public:
	Vector4d(float a= 0, float b =0, float c = 0, float w = 1);
	Vector4d( const Vector4d& v);

	bool operator==(Vector4d& vec);
	bool operator!=(Vector4d& vec);
	float Max()const;
	float Min()const;
	float X()const;
	float Y()const;
	float Z()const;
	float W() const;
	float& W();
	float& X();
	float& Y();
	float& Z();

	float Dot(const Vector4d& vec)const;

	Vector4d Cross( Vector4d& vec )const;
	
	float & operator[](int i ) { return _val[i];}
	const float & operator[](int i ) const { return _val[i];}
	// operators
	bool operator==(const Vector4d & oper)const;
	Vector4d operator+(const Vector4d & oper)const;
	Vector4d operator-(const Vector4d & oper)const;
	Vector4d operator-()const;
	Vector4d operator*(const float & oper)const;
	Vector4d operator/(const float & oper)const;


	Vector4d& operator=(const Vector4d & v);
	Vector4d& operator+=(const Vector4d & oper);
	Vector4d& operator-=(const Vector4d & oper);
	Vector4d& operator*=(const float & oper);
	Vector4d& operator/=(const float & oper);

	Vector4d MultiplyPerElement(Vector4d oper);
	float Size2()const;
	float Size()const;
	void Normalize();
  void Zero();
  Vector4d Scalar(const Vector4d & illumination);
};

#endif