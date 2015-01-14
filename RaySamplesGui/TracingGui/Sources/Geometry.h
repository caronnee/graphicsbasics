#ifndef __MODEL__
#define __MODEL__

#include "Material.h"
#include "Matrix.h"

#define MIN_DIST 0.0001

class Geometry;

struct Intersection
{
	Vector4d nrm;
	Vector4d worldPosition;
	float t;
	Geometry * model;
};

#define BLACK Vector4d(0,0,0,0)
#define WHITE Vector4d(1,1,1,1);

struct Ray
{
	Vector4d origin;
	Vector4d direction;
};

enum Axis
{
	Axis_X,
	Axis_Y,
	Axis_Z
};

enum PropertyType 
{
	PMaterial,
	PMatrix,
	PRadius,
	PPoints
};

#define TYPE_FACTORY(XX)	\
	XX(Point)	\
	XX(Sphere)	\
	XX(Triangle)	\
	XX(Camera)	\

#define CREATE_ENUM(one)	\
	Type##one,

enum TypeId
{
	TYPE_FACTORY(CREATE_ENUM)
	NTypes
};

#include <QString>
#include "FileHandler.h"

class Geometry
{
protected:
	const Material * _material;
	// TODO should be 3x3 matrix?
	Matrix4d _toWorld;
	Matrix4d _toModel;
public:
	Geometry();
	Vector4d& Direction();
	Vector4d& Up();
	Vector4d& Aside();
	void Translate( const Vector4d & vec );
	void Rotate( Axis axis, float angle );
	void Scale( float a, float b, float c);
	void Scale( float a );

	void SetMatrix(const Vector4d& direction, const Vector4d& up);
	Vector4d WorldToModel(const Vector4d & world)const;
	Vector4d ModelToWorld(const Vector4d & modelPosition)const;
	virtual bool Intersect( const Ray & ray, Intersection & sect ) = 0;
	const Material * GetMaterial() const;
	void SetMaterial(const Material * material);
	virtual void SetProperty(PropertyType type, void * value );
	virtual void * GetProperty(PropertyType type);
	void Clear( );
	virtual int Type()const =0;
	// position to illuminate, return the illumination of the point
  virtual Vector4d SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen ) = 0;
  virtual void SaveProperties(FileHandler & handler);
	virtual void LoadProperties(FileHandler & handler);
  virtual Vector4d Radiance(const Vector4d& secNormal,const Vector4d & sampledDir, const float & len);
  virtual float GetDirectionalPdf(const Vector4d & direction, const Vector4d& normal, const Vector4d& pos, const float & len);
};
#endif