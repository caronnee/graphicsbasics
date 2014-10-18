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
	Material * CreateMaterialDiffuse( const Vector4d & diff)
	{
		return new MaterialDiffuse(diff);
	}
};

static MaterialCreator GMaterialCreator;

Material * CreateMaterial(MaterialType type, const Vector4d & parameters)
{
	return GMaterialCreator.CreateMaterialDiffuse(parameters);
}
