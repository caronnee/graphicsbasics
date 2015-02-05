#include "Material.h"
#include "MaterialDiffuse.h"
#include "MaterialSpecular.h"
#include "GuiString.h"
#include "Debug.h"
#include <stdio.h>

#include <QList>

class MaterialCreator
{
  QList<Material *> _materials;
public:
	MaterialCreator()
	{
	}

  Material * CreateMaterialSpecular( const Vector4d & diffuse, const Vector4d & specular, const float & phong )
  {
    Vector4d v = diffuse.MultiplyPerElement(specular);
    Material * m = new MaterialSpecular(v, specular, phong);
    _materials.push_back(m);
    return m;
  }

	Material * CreateMaterialDiffuse( const Vector4d & diff, const Vector4d & emmisive )
	{
    Material * m = new MaterialDiffuse(diff, emmisive);
    _materials.push_back(m);
		return m;
	}
  ~MaterialCreator()
  {
    for ( int i =0; i < _materials.size(); i++)
      delete[] _materials[i];
    _materials.clear();
  }
};

static MaterialCreator GMaterialCreator;

Material * CreateMaterial(int type, const Vector4d * parameters, float phong)
{
  if (type == MSpecular )
    return GMaterialCreator.CreateMaterialSpecular(parameters[0],parameters[1], phong);
  return GMaterialCreator.CreateMaterialDiffuse(parameters[0], parameters[2]);
}

bool Material::IsLight()const
{
  return _emmisive.Size2() > 0;
}

Material::Material(Vector4d e) : _emmisive(e)
{

}

Vector4d Material::Emmisive() const
{
  return _emmisive;
}

float Material::GetDirectionalPdf(const Vector4d &sampledDir, const Vector4d & nrm) const
{
  throw std::logic_error("The method or operation is not implemented.");
}

float Material::Reflectance() const
{
  // does not reflect
  return 0;
}
