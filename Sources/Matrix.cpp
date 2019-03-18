#include "Matrix.h"
#include <math.h>
#include "Debug.h"


Vector4d& Matrix4d::Aside()
{
	return _rows[0];
}

Vector4d& Matrix4d::Up()
{
	return _rows[1];
}
Vector4d& Matrix4d::Direction()
{
	return _rows[2];
}

Matrix4d::Matrix4d()
{
	memset(_rows,0,sizeof(_rows));
}

Matrix4d::Matrix4d(const Matrix4d & matrix)
{
	for ( int i = 0; i < DIM; i++)
		GetRow(i) = matrix.GetRow(i);
}

Matrix4d::Matrix4d(
	float a1, float a2, float a3, float a4, 
	float b1, float b2, float b3, float b4, 
	float c1, float c2, float c3, float c4, 
	float d1, float d2, float d3, float d4 )
{
	GetRow(0) = Vector4d(a1,a2,a3,a4);
	GetRow(1) = Vector4d(b1,b2,b3,b4);
	GetRow(2) = Vector4d(c1,c2,c3,c4);
	GetRow(3) = Vector4d(d1,d2,d3,d4);
}

Matrix4d Matrix4d::Invert()const
{
	Matrix4d d;
	for ( int i =0; i < DIM; i++)
		for ( int j =0; j < DIM; j++)
		{
			float t =SubDeterminant(j,i);
			d.Get(i,j) = SubDeterminant(j,i);
		}
	float det = d.Get(0,0)*Get(0,0) + d.Get(0,1)*Get(1,0) + d.Get(0,2)*Get(2,0) + d.Get(0,3)*Get(3,0);
	if (fabs(det) < 0.001)
	{
		d.Zero();
		return d;
	}
	for (int i =0; i < DIM; i++)
		d.GetRow(i)/=det;
#if _DEBUG
  Matrix4d test = *this * d;
  for ( int i =0; i < DIM; i++)
    for ( int j =0; j < DIM; j++)
    {
      if ( j == i)
      {
        DoAssert(fabs(test.Get(i,j) - 1) < EPSILON);
      }
      else
      {
        DoAssert(fabs(test.Get(i,j)) < EPSILON);
      }
    }
#endif
	return d;
}

Matrix4d& Matrix4d::operator*=(const Matrix4d & oper)
{
	Matrix4d original(*this);
	for (int row = 0; row<DIM; row ++)
	{
		for (int col = 0; col<DIM; col ++)
		{
			float res = 0;;
			for ( int el = 0; el < DIM; el++)
				res += original.GetRow(row)[el]*oper.GetRow(el)[col];
			(*this)[row][col] = res;
		}
	}
	return *this;
}

Matrix4d Matrix4d::operator*(const Matrix4d & oper)const
{
	Matrix4d ret(*this);
	ret*=oper;
	return ret;
}

Vector4d & Matrix4d::operator[](int x)
{
	return GetRow(x);
}

Vector4d& Matrix4d::GetRow(int i)
{
	return _rows[i];
}
const Vector4d Matrix4d::GetRow(int i)const
{
	return _rows[i];
}

void Matrix4d::Identity()
{
	Zero();
	for ( int i = 0; i < DIM; i++)
	{
		GetRow(i)[i] = 1;
	}
}

void Matrix4d::Zero()
{
	for ( int i =0; i < DIM; i++)
		GetRow(i) = Vector4d(0,0,0,0);
}

Vector4d Matrix4d::operator*(const Vector4d& oper)const
{
	Vector4d res;
	for ( int i = 0; i < DIM; i++)
	{
		res[i] = GetRow(i).Dot(oper);
	}
	float weight = res[3];
	if ( weight != 0)
	{
		for ( int i =0; i < DIM; i++) 
		{
			res[i]/=weight;
		}
	}
	return res;
}

Matrix4d& Matrix4d::operator=(Matrix4d & oper)
{
	for (  int i= 0; i<DIM; i++)
	{
		GetRow(i) = oper.GetRow(i);
	}
	return *this;
}

bool Matrix4d::operator==(Matrix4d & oper)
{
	for ( int i =0; i< DIM; i++)
	{
		if ( GetRow(i)!= oper.GetRow(i) )
			return false;
	}
	return true;
}

bool Matrix4d::operator!=(Matrix4d & oper)
{
	bool ret = (*this) == oper;
	return !ret;
}

void Matrix4d::Translate(const Vector4d & trans)
{
	// move in this direction
	Matrix4d translate;
	translate.Identity();
	for (int i =0;i<4; i++)
		translate[i][3] = trans[i];
	translate[3][3] = 1;
	(*this) *= translate;
}

void Matrix4d::RotateAxis( const Vector4d axis,const float & angle)
{
	float c = cos(angle);
	float s = sin(angle);
	Matrix4d m( 1,0,0,0,
		0, c,-s, 0,
		0, s,c, 0,
		0,0,0,1);
	(*this) *=m;
}

void Matrix4d::RotateX(const float & angle)
{
	float c = cos(angle);
	float s = sin(angle);
	Matrix4d m( 1,0,0,0,
		        0, c,-s, 0,
				0, s,c, 0,
				0,0,0,1);
	(*this) *= m;
}

void Matrix4d::RotateY(const float & angle)
{
	float c = cos(angle);
	float s = sin(angle);
	Matrix4d m( c,0,s,0,
		0, 1, 0, 0,
		-s, 0,c, 0,
		0,0,0,1);
	(*this) *=m;
}

void Matrix4d::RotateZ(const float & angle)
{
	float c = cos(angle);
	float s = sin(angle);
	Matrix4d m( c,-s,0,0,
		s, c, 0, 0,
		0, 0,1, 0,
		0,0,0,1);
	(*this) *=m;
}

Matrix4d Matrix4d::Scale(const Vector4d & sc)
{
	Matrix4d scale;
	scale.Identity();
	for ( int i =0; i < DIM; i++)
	{
		scale.GetRow(i)[i] = sc[i];
	}
	return scale;
}

float Matrix4d::Get(int i, int j) const
{
	return GetRow(i)[j];
}

float&Matrix4d::Get(int i, int j)
{
	return GetRow(i)[j];
}

float Matrix4d::SubDeterminant(int a, int b) const
{
	float ret = 0;
	int rowMap[3] = { 0,1,2 };
	int colMap[3] = { 0,1,2 };
	for ( int i = a; i < DIM-1; i++)
		rowMap[i]++;

	for ( int i = b; i < DIM-1; i++)
		colMap[i]++;

	ret = Get( rowMap[0], colMap[0] )*Get( rowMap[1], colMap[1] )*Get( rowMap[2], colMap[2] ) +
		Get( rowMap[0], colMap[1] )*Get( rowMap[1], colMap[2] )*Get( rowMap[2], colMap[0] ) +
		Get( rowMap[0], colMap[2] )*Get( rowMap[1], colMap[0] )*Get( rowMap[2], colMap[1] ) -
		Get( rowMap[0], colMap[2] )*Get( rowMap[1], colMap[1] )*Get( rowMap[2], colMap[0] ) -
		Get( rowMap[0], colMap[1] )*Get( rowMap[1], colMap[0] )*Get( rowMap[2], colMap[2] ) -
		Get( rowMap[0], colMap[0] )*Get( rowMap[1], colMap[2] )*Get( rowMap[2], colMap[1] );
	if ( (a + b)%2 )
		ret *= -1;
	return ret;
}

Matrix4d & Matrix4d::CreateFromZ(const Vector4d & nrm)
{
#if _DEBUG
	float nn = nrm.Size2();
	DoAssert( fabs(nn - 1) < EPSILON );
#endif
  Identity();
	Vector4d aside = (fabs(nrm.X()) > 0.99f) ? Vector4d(0,1,0,0) : Vector4d(1,0,0,0);
	Vector4d direction = nrm;
	Vector4d up = direction.Cross(aside);

  up.Normalize();
	Direction() = direction;
	Up() = up;
	Aside() = Up().Cross(Direction());
	return *this;
}

Vector4d Matrix4d::ToLocal(const Vector4d& v)
{
  Vector4d ret;
  for ( int i =0; i < DIM; i++)
    ret[i] = v.Dot(_rows[i]);
  return ret;
}
