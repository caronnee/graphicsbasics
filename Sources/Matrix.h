#ifndef __MATRIX__
#define __MATRIX__

#include "Vector.h"
#include <string>

class Matrix4d
{
	Vector4d _rows[4];
public:
	Matrix4d();
	Matrix4d( float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4 );
	Matrix4d( const Matrix4d & matrix );
	Vector4d& Aside();
	Vector4d& Up();
	Vector4d& Direction();
	void Identity();
  Vector4d ToLocal( const Vector4d& v);
  Matrix4d Invert()const;
	Vector4d& GetRow(int i);
	const Vector4d GetRow(int i)const;
	Vector4d operator*(const Vector4d& oper)const;
	Matrix4d operator*(const Matrix4d & res)const;
	Vector4d & operator[](int x);
	Matrix4d& operator=(Matrix4d & oper);
	bool operator==(Matrix4d & oper);
	bool operator!=(Matrix4d & oper);
	void Zero();
  Vector4d InSpace(Vector4d & vector)
  {
    return Aside()*vector[0] + Up()*vector[1]+Direction()*vector[2];
  }
	void Translate(const Vector4d & trans);
	void RotateX(const float & angle);
	void RotateY(const float & angle);
	void RotateZ(const float & angle);
	void RotateAxis( const Vector4d axis,const float & angle);
	Matrix4d& Matrix4d::operator*=(const Matrix4d & oper);
	static Matrix4d Scale(const Vector4d & sc);
	float Get(int i, int j) const;
	float & Get(int i, int j);
	float SubDeterminant(int i, int j) const;
	Matrix4d & CreateFromZ(const Vector4d & nrm);
};

#endif