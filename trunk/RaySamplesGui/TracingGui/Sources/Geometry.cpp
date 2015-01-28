#include "Geometry.h"
#include "Debug.h"

Geometry::Geometry() :_material(NULL)
{
	_toWorld.Identity();
	_toModel.Identity();
}


static void checkInvert(const Matrix4d & m1, const Matrix4d & m2)
{
	Matrix4d res = m1*m2;
	for ( int i =0; i <4; i++)
		for ( int j =0; j <4; j++)
		{
			float test = i == j?1:0;
			DoAssert(res[i][j] - test < 0.01);
		}
}

void Geometry::SetMatrix(const Vector4d& direction, const Vector4d& up)
{
	Direction() = direction;
	Direction().Normalize();
	Up() = up;
	Up().Normalize();
	Aside() = Up().Cross(Direction());
	_toWorld = _toModel.Invert();
	checkInvert(_toModel,_toWorld);
}


Vector4d& Geometry::Direction()
{
	return _toModel.Direction();
}

Vector4d& Geometry::Up()
{
	return _toModel.Up();
}

Vector4d& Geometry::Aside()
{
	return _toModel.Aside();
}


Vector4d Geometry::WorldToModel(const Vector4d & world)const
{
	Vector4d ret = _toModel * world;
	return ret;
}


Vector4d Geometry::ModelToWorld(const Vector4d & modelPosition) const
{
	Vector4d ret = _toWorld * modelPosition;
	return ret;
}

void Geometry::Translate(const Vector4d & direction)
{
	_toModel.Translate(direction);
	_toWorld = _toModel.Invert();
	checkInvert(_toModel,_toWorld);
}

const Material * Geometry::GetMaterial() const
{
	return _material;
}

void Geometry::SetMaterial(const Material * material)
{
	_material = material;
}

void Geometry::Rotate(Axis axis, float angle)
{
	float cs = cos(angle);
	float sn = sin(angle);
	Matrix4d rotMatrix;
	rotMatrix.Identity();
	int first = (axis != Axis_X)? 0:1;
	int second = (axis != Axis_Z)? 2:1;
	rotMatrix.GetRow(first)[first] = cs;
	rotMatrix.GetRow(second)[second] = cs;
	rotMatrix.GetRow(first)[second] = sn;
	rotMatrix.GetRow(second)[first] = -sn;
	_toModel *= rotMatrix;
	_toWorld = _toModel.Invert();
	checkInvert(_toModel,_toWorld);
}

void Geometry::Scale( float a, float b, float c)
{
	_toModel.Scale(Vector4d(a,b,c,1));
	_toWorld = _toModel.Invert();
	checkInvert(_toModel,_toWorld);
}

void Geometry::Scale( float a )
{
	_toModel.Scale(Vector4d(a,a,a,1));
	_toWorld = _toModel.Invert();
	checkInvert(_toModel,_toWorld);
}

void Geometry::SetProperty(PropertyType type, void * value)
{
	switch (type)
	{
	case PMaterial:
		{
			Material * mat = (Material *)value;
			SetMaterial(mat);
			break;
		}
	case PMatrix:
		{
			Vector4d * m = (Vector4d *) value;
			SetMatrix(m[0],m[1]);
		}
	default:
		break;
	}
}

void Geometry::Clear()
{
	_toModel.Identity();
	_toWorld.Identity();
}

void * Geometry::GetProperty(PropertyType type)
{
	return NULL;
}

void Geometry::SaveProperties(FileHandler & handler)
{
	// nothing
}

void Geometry::LoadProperties(FileHandler & handler)
{
	// nothing
}

Vector4d Geometry::Radiance(const Vector4d& secNormal,const Vector4d & sampledDir, const float & len)
{
//  throw "not implemented";
  return Vector4d(0,0,0,0);
}

float Geometry::GetDirectionalPdf(const Vector4d & direction, const Vector4d& normal, const Vector4d& pos, const float & len)
{
  throw std::logic_error("The method or operation is not implemented.");
}

float Geometry::Area() const
{
  throw std::logic_error("The method or operation is not implemented.");
}
