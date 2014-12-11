#include "scene.h"
#include "Plane.h"
#include "FileHandler.h"

AmbientLight& Scene::Ambient()
{
	return _ambientLight;
}

Scene::Scene() :_ambientLight(NULL), _camera(NULL)
{

}

#if _DEBUG
extern bool stop;
#endif

bool Scene::FindIntersection(const Ray & ray, Intersection& res)
{
	bool found = false;
	res.t = 1e31f;

	// TODO optimize
	for (size_t i =0; i<_geometry.size(); i++)
	{
		Intersection isect;
		Geometry & geom = * _geometry[i];
		
		if ( geom.Intersect(ray,isect) )
		{
			if ( res.t < 0.001)
				continue;
			if ( isect.t < res.t ) // the nearest the best
				res = isect;
			found = true;
		}
	}
	return found;
}

void Scene::DeleteModel(Geometry * geometry)
{
	int index = 0;
	while ( index < _geometry.size() )
	{
		if (_geometry[index] == geometry)
		{
			_geometry[index] = _geometry.back();
			_geometry.pop_back();
		}
		else
			index++;
	}
}

Camera * GCamera;
Camera * GetCamera()
{
  return GCamera;
}

void Scene::CreateCamera( CameraContext & ctx)
{
	Camera * c = new Camera();
	c->Clear();
	c->SetMatrix(ctx.axis[0],ctx.axis[1]);
	c->Translate(ctx.position);
	float zFar = 1000;
	c->SetPerspective(zFar,ctx.fov);
	// Vector4d r = c->RasterToWorld(Vector4d(0.5,0.5,0,1));
	if (_camera)
	{
		DeleteModel(_camera);
		delete _camera;
	}
  GCamera = c;
  c->RasterToWorld(Vector4d(256,256));
	_camera =c;
	Vector4d v[] = {Vector4d(0,0,0,0),Vector4d(0,0,0,0)};
	_camera->SetMaterial(CreateMaterial(MDiffuse,v,0));
	_camera->SetResolution(ctx.resolution[0],ctx.resolution[1]);
	AddModel(_camera);
}

#include "AmbientLight.h"

void Scene::AddModel(Geometry * model)
{
	_geometry.push_back(model);
	if (model->GetMaterial()->IsLight())
	{
		_emmiters.push_back(model);
	}
}

void Scene::Clear()
{
	_ambientLight.SetPower(Vector4d(0,0,0,0));
	_emmiters.clear();
	_emmiters.push_back(&_ambientLight);
	_geometry.clear();
}

int Scene::Lights()
{
	return _emmiters.size();
}

Geometry * Scene::GetLight(int iLight)
{
	return _emmiters[iLight];
}

Ray Scene::GetRay(float x,float y)
{
	return _camera->GetRay(x,y);
}
