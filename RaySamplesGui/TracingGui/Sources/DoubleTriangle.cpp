#include "DoubleTriangle.h"

DoubleTriangle::DoubleTriangle(Vector4d * trianglePoints)
{
	static Vector4d def [] ={ Vector4d(1,0,0,1),Vector4d(0,1,0,1),Vector4d(1,1,0,1)};
	if ( trianglePoints == NULL )
	{
		trianglePoints = def;
	}
	SetPoints(trianglePoints);
}

void DoubleTriangle::SetPoints(Vector4d * trianglePoints)
{
	memcpy(_points, trianglePoints, sizeof(Vector4d) * 3 );
	_edges[0] = _points[1] -  _points[0];
	_edges[1] = _points[2] -  _points[0];
	_normal = _edges[1].Cross(_edges[0]);
	_area = 0.5f*_normal.Size();
	_normal.Normalize();
}
void DoubleTriangle::SetProperty(PropertyType type, void * values)
{
	if (type == PPoints)
	{
		SetPoints((Vector4d*)values);
	}
}

void * DoubleTriangle::GetProperty(PropertyType type)
{
	if (type == PPoints)
	{
		return _points;
	}
	return NULL;
}

bool DoubleTriangle::Intersect(const Ray & ray, Intersection & sect)
{
	//M�ller�Trumbore_intersection_algorithm
	Vector4d d = WorldToModel(ray.direction);
	d.Normalize();
	Vector4d o = WorldToModel(ray.origin);
	//Begin calculating determinant - also used to calculate u parameter
	Vector4d P = d.Cross(_edges[1]);
	
	//if determinant is near zero, ray lies in plane of triangle
	float det = _edges[0].Dot(P);
	//NOT CULLING
	if(det > -EPSILON && det < EPSILON) 
		return false;

	float inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	Vector4d T =  o - _points[0];

	//Calculate u parameter and test bound
	float u = T.Dot(P) * inv_det;
	//The intersection lies outside of the triangle
	if(u < 0.f || u > 1.f) 
		return false;

	//Prepare to test v parameter
	Vector4d Q = T.Cross(_edges[0]);

	//Calculate V parameter and test bound
	float v = d.Dot(Q) * inv_det;

	//The intersection lies outside of the triangle
	if(v < 0.f || u + v  > 1.f) 
		return false;

	float t = _edges[1].Dot(Q) * inv_det;

	if(t > EPSILON) { //ray intersection
		sect.t = t;
		sect.model = this;
		sect.worldPosition = ray.origin + ray.direction * t;
		sect.nrm = ModelToWorld(_normal);
		sect.nrm.Normalize();
		return true;
	}

	// No hit, no win
	return false;
}

int DoubleTriangle::Type()const
{
	return TypeTriangle;
}

#include "RandomNumber.h"

float lasta;
float lasbB;
Vector4d DoubleTriangle::SampleIllumination(Intersection &section, Vector4d & sampledDir, float & len)
{
	// sample point on the triangle
	float a1 = GetFloat();
	float a2 = 0;
	if (a1<1)
	 a2 = GetFloat() * (1.0f-a1);
	lasta = a1;
	lasbB = a2;
	Vector4d point = _edges[0]*a1 + _edges[1]*a2 + _points[0];
	Vector4d mPoint = ModelToWorld(point);
	sampledDir = mPoint-section.worldPosition;
	len = sampledDir.Size();
	sampledDir.Normalize();
	float d2 = len * len;
	float cosA = section.nrm.Dot(sampledDir);
	float cosB = -_normal.Dot(sampledDir);
	if ( (cosA <= 0) || (cosB <= 0))
		return Vector4d(0,0,0,0);
	Vector4d v = GetMaterial()->Emmisive();
	return v * cosA * cosB * _area /d2;
}

void DoubleTriangle::SaveProperties(FileHandler & handler)
{
	handler.Write(_points,sizeof(Vector4d),3);	
}

void DoubleTriangle::LoadProperties(FileHandler & handler)
{
	handler.Read(_points,sizeof(Vector4d),3);
	SetProperty(PPoints,_points);
}
