#include "Material.h"
#include "MaterialDiffuse.h"
#include "MaterialSpecular.h"
#include "GuiString.h"
#include "Debug.h"
#include <stdio.h>

#include <QList>

class MaterialCreator
{

public:
	MaterialCreator()
	{
	}
	Material * CreateMaterialDiffuse( const Vector4d & diff, const Vector4d & emmisive )
	{
		return new MaterialDiffuse(diff,emmisive);
	}
};

static MaterialCreator GMaterialCreator;

Material * CreateMaterial(MaterialType type, const Vector4d & parameters, const Vector4d & emmisive)
{
	return GMaterialCreator.CreateMaterialDiffuse(parameters, emmisive);
}

bool Material::IsLight()
{
  return _emmisive.Size2() > 0;
}

Material::Material(Vector4d e) : _emmisive(e)
{

}
