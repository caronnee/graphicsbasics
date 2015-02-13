#include "scene.h"
#include "Plane.h"
#include "FileHandler.h"

AmbientLight& Scene::Ambient()
{
	return _ambientLight;
}

Scene::Scene() :_ambientLight(NULL), _camera(NULL), _totalArea(0)
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
		Geometry * geom = _geometry[i];
		
		if ( geom->Intersect(ray,isect) )
		{
			if ( res.t < EPSILON)
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

// create and seupt camera
Camera * CreateCamera( CameraContext & ctx)
{
  Camera * c = new Camera();
  c->Clear();
  c->SetMatrix(ctx.axis[0],ctx.axis[1]);
  c->Translate(ctx.position);
  float zFar = 1000;
  c->SetPerspective(zFar,ctx.fov);
  c->SetResolution(ctx.resolution[0],ctx.resolution[1]);  
  return c;
}

Camera * Scene::SwitchCamera( Camera * camera )
{
  Camera * c = _camera;
  for ( int i =0; i  < _geometry.size(); i++)
  {
    if ( c == _geometry[i])
    {
      _geometry[i] = _geometry.back();
      _geometry.pop_back();
      break;
    }
  }
  _camera = camera;
  return c;
}
void Scene::CreateAddCamera( CameraContext & ctx)
{
	Camera * c = CreateCamera(ctx);

	if (_camera)
	{
		DeleteModel(_camera);
		delete _camera;
	}
  _camera =c;
	Vector4d v[] = {Vector4d(0,0,0,0),Vector4d(0,0,0,0),Vector4d(0,0,0,0)};
	_camera->SetMaterial(CreateMaterial(MDiffuse,v,0));
	AddModel(_camera);
}

#include "AmbientLight.h"

void Scene::AddModel( Geometry * model )
{
	_geometry.push_back(model);
	if ( model->GetMaterial()->IsLight() )
	{
		_emmiters.push_back(model);
    _totalArea += model->Area();
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

const float Scene::TotalArea() const
{
  return _totalArea;
}

Geometry * Scene::Model(int i) const
{
  if ( i < _geometry.size())
    return _geometry[i];
  return NULL;
}
