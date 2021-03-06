#include "Sphere.h"
#include "Debug.h"

Sphere::Sphere(float radius) :Geometry()
{
	SetProperty(PRadius, &radius);
}

template<typename T>
bool solveQuadratic(const T &a, const T &b, const T &c, T &x0, T &x1)
{
	T discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = - 0.5 * b / a;
	else {
		T q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) :
		-0.5 * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);
	return true;
}

bool Sphere::Intersect(const Ray & ray, Intersection & sect)
{
	// analytic solution
  DoAssert( (ray.direction.Size2() - 1) < EPSILON);
	Vector4d rayDirection = WorldToModel(ray.direction);
	Vector4d rayOrigin = WorldToModel( ray.origin );
	Vector4d L = Vector4d(0,0,0,1) - rayOrigin;	
	//float a = rayDirection.Dot(rayDirection);
	//float b = 2 * rayDirection.Dot(L);
	//float c = L.Dot(L) - 1;
	//float t1,t0;
	//if (!solveQuadratic<float>(a, -b, c, t0, t1)) 
	//	return false;

	//// geometric solution
	DoAssert( fabs(1 - rayDirection.Size2()) <  EPSILON);
	float tca = L.Dot(rayDirection);
	if (tca < 0) // behind the ray
		return false;
	float d2 = L.Dot(L) - tca * tca;
	if ( d2 > _radius2 ) // outside of radius
		return false;
	float thc = sqrt(_radius2 - d2);
	float t0 = tca - thc;
	float t1 = tca + thc;
	sect.model = this;
	t0 = t0>0 ? t0:t1;
	sect.t = t0;
	sect.worldPosition = ray.origin + ray.direction * t0;
	sect.nrm = rayOrigin + rayDirection * t0 - Vector4d(0,0,0,1);
	sect.nrm = ModelToWorld(sect.nrm);
	//verify that this works
	{
		Vector4d otherNorm = sect.worldPosition - ModelToWorld(Vector4d(0,0,0,1));
		float siz2 = (otherNorm - sect.nrm).Size2();
#if _DEBUG
    if (siz2 > EPSILON )
			__debugbreak();
#endif
		float dist2 = WorldToModel(sect.worldPosition).Size2();
		Vector4d testIntersection = sect.worldPosition;// ray.origin + ray.direction * t0;
		Vector4d sphereCenter = ModelToWorld(Vector4d(0,0,0,1));
		float sz = (sphereCenter - testIntersection ).Size2();;
		DoAssert(sz-_radius2<=2*EPSILON);
	}
	sect.nrm.Normalize();
	return true;
}

int Sphere::Type()const
{
	return TypeSphere;
}

void * Sphere::GetProperty(PropertyType type)
{
	if ( type == PRadius)
		return &_radius;
	return base::GetProperty(type);
}

void Sphere::SetProperty(PropertyType type, void * value)
{
	if ( type == PRadius)
	{
		_radius = *(float*)value;
		_radius2 = _radius*_radius;
	}
	base::SetProperty(type,value);
}

Vector4d Sphere::SampleIllumination(const Intersection &section, Vector4d & sampledDir, float & sampleLen)
{
  DoAssert(false);
	throw "Not implemented yet. And maybe won't be ever...";
}

void Sphere::SaveProperties(FileHandler & handler)
{
	handler.Write( &_radius, sizeof (float),1 );
}

void Sphere::LoadProperties(FileHandler & handler)
{
	handler.Read( &_radius,sizeof(_radius),1);
	SetProperty(PRadius, &_radius);
}

#include "MathUtil.h"

void Sphere::GenerateSurfels(std::vector<Surfel> & surfels, const int & grain)
{
  Surfel surf(this);
  surf.color = Vector4d(0,0,0,0);

  float step = PI/(0.5*grain);  
  float smallR = 0.5*_radius/ tan (step);
  surf.radius = smallR;
  surf.area = PI * surf.radius*surf.radius;

  for ( float i =0; i< 2*PI; i+=step)
    for ( float j =0; j< 2*PI; j+=step)
    {
      Vector4d pos = GetSpherePosition(i,j)*_radius;
      pos[3] = 1;
      surf.position = ModelToWorld(pos);
      pos[3] = 0;
      surf.normal = ModelToWorld(pos);
      surf.normal.Normalize();
      surfels.push_back(surf);
    }
}
