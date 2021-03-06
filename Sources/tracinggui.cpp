#include <QTCore/QAbstractListModel>

#include "tracinggui.h"
#include "TrackProgress.h"
#include "Camera.h"
#include "scene.h"
#include "testRenderer.h"
#include "Plane.h"
#include "AmbientLight.h"
#include <QTGui/qpainter.h>
#include "Tonemapper.h"
#include "Debug.h"

#define CREATE_NAMES(name) #name,

static const char * typeNames[] = { TYPE_FACTORY(CREATE_NAMES) };

GModelObjects::GModelObjects(QObject *parent) : QAbstractListModel(parent)
{
}

int GModelObjects::rowCount(const QModelIndex & /*parent*/) const
{
	return _geometries.size();
}

int GModelObjects::columnCount(const QModelIndex & /*parent*/) const
{
	return 1;
}

QVariant GModelObjects::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(orientation == Qt::Horizontal)
	{
		switch(section)
		{
		case 0:
			return ("Scene Desc");

		default:
			return QVariant();
		}
	}
	return QVariant();
}

QVariant GModelObjects::data(const QModelIndex &index, int role) const
{
	if (index.row() >= _geometries.size())
		return QVariant();
	if (role == Qt::DisplayRole || role == Qt::EditRole )
	{
		int rindex = 0;
		return _geometries[index.row()].name;
	}
	return QVariant();
}

QModelIndex GModelObjects::Add(Geometry * geom, int index)
{
	if (index < 0)
		index = _geometries.size();
	GProperties* pprop;
	QModelIndex ret = createIndex(index,0);
	
	if(index >= _geometries.size())
	{
		beginInsertRows(QModelIndex(), _geometries.size(), _geometries.size() + 1);
		GProperties tprop;
		_geometries.push_back(tprop);
		endInsertRows();
		emit dataChanged(createIndex(0,0),ret);
	}
	else
		delete _geometries[index].geom;
	pprop = & _geometries[index];
	GProperties& prop = *pprop;
	prop.position = Vector4d(0,0,0,0);
	prop.diffuse = Vector4d(1,1,1,1);
	prop.geom = geom;
	prop.name = typeNames[geom->Type()];
	return ret;
}

bool GModelObjects::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (role == Qt::EditRole)
	{
		GProperties& prop = _geometries[index.row()];		
		//save value from editor to member m_gridData
		prop.name = value.toString();
	}
	return true;
}

Qt::ItemFlags GModelObjects::flags(const QModelIndex & /*index*/) const
{
	return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
}

GProperties * GModelObjects::Get(int row)
{
	if ( row >= _geometries.size())
		return NULL;
	return &_geometries[row];
}

void GModelObjects::Clear()
{
	_geometries.clear();
}

void GModelObjects::Remove(int index)
{
	_geometries.removeAt(index);
}

void GModelObjects::resize(int i)
{
	while ( i< _geometries.size())
	{
		_geometries.removeAt(i);
	}
}

SceneModels GScenes;

TracingGui::TracingGui(QWidget *parent)
	: QMainWindow(parent), _image(),_gModels(NULL)
{
	ui.setupUi(this);
	// connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	// all connections will be wet up here
	connect(ui.bRender, SIGNAL(clicked()),this, SLOT(RenderSlot()) );
	connect(ui.bSave, SIGNAL(clicked()),this, SLOT(SaveImageSlot()) );
	connect(ui.sExposure, SIGNAL(valueChanged(int)), this, SLOT(ExposureChangedSlot(int)));

	TrackProgress * GetRendererTrack();
	connect(GetRendererTrack(),SIGNAL(Progress(int)),ui.renderProgress,SLOT(setValue(int)));
	connect(ui.sExposureNumber,SIGNAL(valueChanged(double)),this,SLOT(UpdateExposureSlot(double)));
	connect(ui.sExposure,SIGNAL(valueChanged(int)),this,SLOT(UpdateExposureNumberSlot(int)));

	connect(ui.sZoomNumber,SIGNAL(valueChanged(int)),ui.sZoom,SLOT(setValue(int)));
	connect(ui.sZoom,SIGNAL(valueChanged(int)),ui.sZoomNumber,SLOT(setValue(int)));

	/******************* camera tab connections *****************88*/
	connect(ui.xRotation,SIGNAL(sliderMoved(int)),ui.xRotationDeg,SLOT(setValue(int)));
	connect(ui.xRotation,SIGNAL(valueChanged(int)),ui.xRotationRad,SLOT(setValue(int)));
	connect(ui.xRotationDeg,SIGNAL(valueChanged(int)),ui.xRotation,SLOT(setValue(int)));

	connect(ui.yRotation,SIGNAL(sliderMoved(int)),ui.yRotationDeg,SLOT(setValue(int)));
	connect(ui.yRotation,SIGNAL(valueChanged(int)),ui.yRotationRad,SLOT(setValue(int)));
	connect(ui.yRotationDeg,SIGNAL(valueChanged(int)),ui.yRotation,SLOT(setValue(int)));

	connect(ui.zRotation,SIGNAL(sliderMoved(int)),ui.zRotationDeg,SLOT(setValue(int)));
	connect(ui.zRotation,SIGNAL(valueChanged(int)),ui.zRotationRad,SLOT(setValue(int)));
	connect(ui.zRotationDeg,SIGNAL(valueChanged(int)),ui.zRotation,SLOT(setValue(int)));

	// camera position
	connect(ui.xzRot,SIGNAL(sliderMoved(int)),ui.xzRotDeg,SLOT(setValue(int)));
	connect(ui.xzRot,SIGNAL(valueChanged(int)),ui.xzRotRad,SLOT(setValue(int)));
	connect(ui.xzRotDeg,SIGNAL(valueChanged(int)),ui.xzRot,SLOT(setValue(int)));

	connect(ui.xPos,SIGNAL(sliderMoved(int)),ui.xPosVal,SLOT(setValue(int)));
	connect(ui.yPos,SIGNAL(sliderMoved(int)),ui.yPosVal,SLOT(setValue(int)));
	connect(ui.zPos,SIGNAL(sliderMoved(int)),ui.zPosVal,SLOT(setValue(int)));

	// direction
	connect(ui.xDir,SIGNAL(sliderMoved(int)),ui.xDirVal,SLOT(setValue(int)));
	connect(ui.yDir,SIGNAL(sliderMoved(int)),ui.yDirVal,SLOT(setValue(int)));
	connect(ui.zDir,SIGNAL(sliderMoved(int)),ui.zDirVal,SLOT(setValue(int)));

	// init Values
	ui.xDirVal->setValue(0.0f);
	ui.yDirVal->setValue(0.0f);
	ui.zDirVal->setValue(10.0f);

	// fov
	connect(ui.fov,SIGNAL(valueChanged(int)),ui.fovDeg,SLOT(setValue(int)));
	connect(ui.fov,SIGNAL(valueChanged(int)),ui.fovRad,SLOT(setValue(int)));
	connect(ui.fovDeg,SIGNAL(valueChanged(int)),ui.fov,SLOT(setValue(int)));

	// init Values
	ui.fov->setValue(95);

	// connect button to create export/import
	connect(ui.exportScene, SIGNAL(clicked(void)),this,SLOT(SaveNewSceneSlot()));
	connect(ui.exportMaterial, SIGNAL(clicked(void)), this, SLOT( SaveMaterialSlot()));
	connect(ui.saveScene, SIGNAL(clicked(void)),this,SLOT(SaveCurrentSceneSlot()));
	connect(ui.saveMaterial, SIGNAL(clicked(void)),this,SLOT(SaveCurrentMaterialSlot()));

	// connect buttons to create types
  connect( ui.addSphere, SIGNAL(clicked(void)), this, SLOT(AddSphereSlot()));
  connect( ui.addTorus, SIGNAL(clicked(void)), this, SLOT(AddTorusSlot()));
	connect( ui.delObject, SIGNAL(clicked(void)), this, SLOT(DeleteObjectSlot()));
	connect( ui.addPoint, SIGNAL(clicked(void)), this, SLOT(AddPointSlot()));

	_gModels = new GModelObjects(NULL);
	ui.treeView->setModel(_gModels);
	ui.treeViewLight->setModel(_gModels);
	//selection changes shall trigger a slot
	connect(ui.treeView->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
		this, SLOT(SelectionModelChangedSlot(const QItemSelection &, const QItemSelection &)));
	connect(ui.treeViewLight->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
		this, SLOT(SelectionMaterialChangedSlot(const QItemSelection &, const QItemSelection &)));

	// renderer
  ui.rendererType->addItem("SurfelRenderer",QVariant(2));	
  ui.rendererType->addItem("PathtracerRenderer",QVariant(1));	
	ui.rendererType->addItem("TestRenderer",QVariant(0));

  // what to calculate
  ui.directType->addItem("None",QVariant(RNone));
  ui.directType->addItem("Direct light",QVariant(RDirectLight));
  ui.directType->addItem("Direct light BRDF",QVariant(RDirectBRDF));
  ui.directType->addItem("Direct light Uniform",QVariant(RDirectUniform));
  ui.directType->addItem("Direct light MIS",QVariant(RDirectMIS));

  ui.indirectType->addItem("None",QVariant(RNone));
  ui.indirectType->addItem("Indirect light ( Fixed bounces )",QVariant(RIndirectLightBounced));
  ui.indirectType->addItem("Indirect light ( Simple gather )",QVariant(RIndirectSimple));
  ui.indirectType->addItem("Indirect light ( Next event estimation )",QVariant(RIndirectNextEvent));
  ui.indirectType->addItem("Indirect light ( MIS )",QVariant(RIndirectMIS));

  // scenes selections
  LoadSceneNames();
  ui.sceneNames->setModel(&GScenes);
  connect(ui.sceneNames->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
	  this, SLOT(SelectionSceneChangedSlot(const QItemSelection &, const QItemSelection &)));

  connect(ui.fixedX,SIGNAL(valueChanged(int)), this, SLOT(ShowFixedSlot(void)) );
  connect(ui.fixedY,SIGNAL(valueChanged(int)), this, SLOT(ShowFixedSlot(void)) );
  // connect min/max on fixed point
  //connect ( ui.fix)

  LoadSetup();

  memset(_threads,0,sizeof(_threads));
  LoadModels(ui.currentScene->text());
  LoadMaterials(ui.currentMaterial->text());
  LoadCamera();
}

#include "Sphere.h"

void TracingGui::SelectionMaterialChangedSlot(const QItemSelection & newSel, const QItemSelection &oldSel)
{
	UpdateSelectedModel(oldSel.indexes(),1);
	QModelIndexList list = newSel.indexes();
	if ( list.size() == 0 )
		return;

	int index = list[0].row();
	GProperties & gProp = *_gModels->Get(index);

	//ui.MaterialType 
	ui.dR->setValue(gProp.diffuse[0] );
	ui.dG->setValue(gProp.diffuse[1] );
	ui.dB->setValue(gProp.diffuse[2] );

	ui.sR->setValue(gProp.specular[0] );
	ui.sG->setValue(gProp.specular[1] );
	ui.sB->setValue(gProp.specular[2] );
	ui.sE->setValue(gProp.matSpecularExp);

	ui.eR->setValue(gProp.emmisive[0] );
	ui.eG->setValue(gProp.emmisive[1] );
	ui.eB->setValue(gProp.emmisive[2] );
}
void TracingGui::SelectionModelChangedSlot(const QItemSelection & newSel, const QItemSelection &oldSel)
{
	UpdateSelectedModel(oldSel.indexes(),0);
	// fill from new index
	QModelIndexList list = newSel.indexes();
	if ( list.size() == 0 )
		return;
	int index = list[0].row();
	GProperties & gProp = *_gModels->Get(index);
	//gProp.color
	ui.xModelPos->setValue( gProp.position[0] );
	ui.yModelPos->setValue( gProp.position[1] );
	ui.zModelPos->setValue( gProp.position[2] );
	ui.xRotationDeg->setValue( gProp.rotation[0] );
	ui.yRotationDeg->setValue( gProp.rotation[1] );
	ui.zRotationDeg->setValue( gProp.rotation[2] );
	static float defRadius = 0;
	float * r = (float *)gProp.geom->GetProperty(PRadius);
	if (!r)
		r = &defRadius;
	
	ui.radius->setValue( *r );
  r = (float *)gProp.geom->GetProperty(PTubeRadius);
  if (!r)
    r = &defRadius;
  ui.tuberadius->setValue(*r);
	// position
	Vector4d * vectors = (Vector4d *)gProp.geom->GetProperty(PPoints);
	static Vector4d def [] = {Vector4d(0,0,0,1),Vector4d(0,0,0,1),Vector4d(0,0,0,1)};
	if ( !vectors )
	{
		vectors = def;
	}

	ui.xVert_1->setValue(vectors[0][0]);
	ui.yVert_1->setValue(vectors[0][1]);
	ui.zVert_1->setValue(vectors[0][2]);
	
	ui.xVert_2->setValue(vectors[1][0]);
	ui.yVert_2->setValue(vectors[1][1]);
	ui.zVert_2->setValue(vectors[1][2]);

	ui.xVert_3->setValue(vectors[2][0]);
	ui.yVert_3->setValue(vectors[2][1]);
	ui.zVert_3->setValue(vectors[2][2]);

}

#include "Torus.h"

void TracingGui::AddTorusSlot()
{
  Geometry * geom = new Torus(2,1);
  QModelIndex index = _gModels->Add(geom, -1);
  ui.treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}
void TracingGui::AddSphereSlot()
{
	Geometry * geom = new Sphere(1);
	QModelIndex index = _gModels->Add(geom,-1);
	ui.treeView->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
}

#include "doubleTriangle.h"
#include "FileHandler.h"
#define DEFAULT_SCENE "../../data/test.scene"
#define DEFAULT_CAMERA "../../data/scene.camera"
#define DEFAULT_PATH "../../data/"

void TracingGui::LoadMaterials(const QString & nname)
{
	ui.treeView->selectionModel()->clear();
	ui.treeViewLight->selectionModel()->clear();
	QString name = nname;
	name.prepend(DEFAULT_PATH);
	FileHandler handler;
	handler.Open(name.toLocal8Bit().data(),"r");
	if ( !handler.Valid())
		return;

	int index = 0;
	while (true)
	{
		GProperties *g = _gModels->Get(index);
		if (!g)
			break;
		index++;
		handler.Read( &g->diffuse, sizeof (g->diffuse), 1);
		handler.Read( &g->specular, sizeof (g->specular), 1);
		handler.Read( &g->matSpecularExp, sizeof (g->matSpecularExp), 1);
		handler.Read( &g->emmisive, sizeof (g->emmisive), 1);
	}
	float ambient[3];
	handler.Read( &ambient, sizeof (float), 3 );
	ui.aRValue->setValue(ambient[0]);
	ui.aGValue->setValue(ambient[1]);
	ui.aBValue->setValue(ambient[2]);
}

void TracingGui::LoadModels(const QString & nname)
{
	ui.treeView->selectionModel()->clear();
	ui.treeViewLight->selectionModel()->clear();
	QString name = nname;
	name.prepend(DEFAULT_PATH);

	FileHandler handler;
	handler.Open(name.toLocal8Bit().data(),"r");
	if ( !handler.Valid())
		return;
	int positionManual = 0;

	int i=0;
	while (true)
	{
		int w;
		handler.Read(&w,sizeof(int),1);
		if (handler.End())
			break;
		Geometry * geom;
		bool points = false;
		bool hasRadius = false;
    if (w == TypeSphere)
    {
      hasRadius = true;
      positionManual++;
      geom = new Sphere(1);
    }
    else if (w == TypeTriangle)
    {
      geom = new DoubleTriangle(NULL);
      points = true;
    }
    else if (w == TypePoint)
    {
      geom = new PointObject();
    }
    else if (w == TypeTorus)
      geom = new Torus(0,0);
		else
		{
			DoAssert(false);
		}
		QModelIndex index = _gModels->Add(geom,i);
		i++;
		GProperties * g = _gModels->Get(index.row());

		int l;
		handler.Read(&l,sizeof(int),1);
		char buffer[256];
		handler.Read(buffer,1,l);
		buffer[l] = '\0';
		g->name = buffer;
		handler.Read( &g->position, sizeof (g->position), 1 );

		if (positionManual == -1)
		{
			Vector4d cb[]= {
				Vector4d(-1.27029f,  1.30455f, -1.28002f),
				Vector4d( 1.28975f,  1.30455f, -1.28002f),
				Vector4d( 1.28975f,  1.30455f,  1.28002f),
				Vector4d(-1.27029f,  1.30455f,  1.28002f),
				Vector4d(-1.27029f, -1.25549f, -1.28002f),
				Vector4d( 1.28975f, -1.25549f, -1.28002f),
				Vector4d( 1.28975f, -1.25549f,  1.28002f),
				Vector4d(-1.27029f, -1.25549f,  1.28002f) };

			float smallRadius = 0.5f;
			Vector4d leftWallCenter  = (cb[0] + cb[4]) * (1.f / 2.f) + Vector4d(0, 0, smallRadius);
			Vector4d rightWallCenter = (cb[1] + cb[5]) * (1.f / 2.f) + Vector4d(0, 0, smallRadius);
			float xlen = rightWallCenter[0] - leftWallCenter[0];
			Vector4d leftBallCenter  = leftWallCenter  + Vector4d(2.f * xlen / 7.f, 0, 0);
			Vector4d rightBallCenter = rightWallCenter - Vector4d(2.f * xlen / 7.f, -xlen/4, 0);

			Vector4d final = rightBallCenter;
			float temp = final[1];
			final[1] = final[2];
			final[2] = temp;
			g->position = final;
		}

		handler.Read( &g->rotation, sizeof (g->rotation), 1);
		geom->LoadProperties(handler);

	}
	_gModels->resize(i);
}

void TracingGui::SaveMaterials(const QString & materialName)
{
	FileHandler handler;
	if (handler.Open(materialName.toLocal8Bit().data(),"w") == false)
		return;

	int i =0;
	GProperties * g;
	while ( g = _gModels->Get(i))
	{
		// material related
//		handler.Write( &g->material, sizeof (g->material),1 );
		handler.Write( &g->diffuse, sizeof (g->diffuse),1 );
		handler.Write( &g->specular, sizeof (g->specular),1 );
		handler.Write( &g->matSpecularExp, sizeof (g->matSpecularExp),1 );
		handler.Write( &g->emmisive, sizeof (g->emmisive),1 );
		i++;
	}
	float ambient[]	=	{	ui.aRValue->value(),ui.aGValue->value(),ui.aBValue->value()	};
	handler.Write( ambient, sizeof (float), 3 );
}
// saves material to the new slot and picks it
void TracingGui::SaveMaterialSlot( )
{
	QString materialName = GScenes.NewMaterialName();
	materialName.prepend(DEFAULT_PATH);
	UpdateSelectedModel(ui.treeViewLight->selectionModel()->selectedIndexes(),1);
	SaveMaterials(materialName);	
}

void TracingGui::SaveModels(const QString & str )
{
	UpdateSelectedModel(ui.treeView->selectionModel()->selectedIndexes(),0);

	FileHandler handler;
	handler.Open(str.toLocal8Bit().data(),"w");

	int i =0;
	GProperties * g;
	while ( g = _gModels->Get(i))
	{
		Geometry * geom = g->geom;

		int tp = geom->Type();
		handler.Write(&tp,1,sizeof(int));
		int len = g->name.length();
		handler.Write(&len,sizeof(int),1);
		handler.Write(g->name.toLocal8Bit().data(),1,g->name.length());
		handler.Write( &g->position, sizeof (g->position),1 );
		handler.Write( &g->rotation, sizeof (g->rotation),1 );
		geom->SaveProperties(handler);

		i++;
	}
}

void TracingGui::SaveCamera()
{
	FileHandler handler;
	handler.Open(DEFAULT_CAMERA,"w");

	handler.Write( (float)ui.xPosVal->value() );
	handler.Write( (float)ui.yPosVal->value() );
	handler.Write( (float)ui.zPosVal->value() );

	handler.Write((float)ui.xDirVal->value());
	handler.Write((float)ui.yDirVal->value());
	handler.Write((float)ui.zDirVal->value());

	handler.Write(ui.xzRotDeg->value());
	handler.Write(ui.fovDeg->value());
}

void TracingGui::ShowFixedSlot()
{
  if ( (_image.W() < ui.fixedX->value()) || (_image.H() < ui.fixedY->value()) )
    return;

  if ( (_prevX > 0) && (_prevY > 0) )
  {
    _image.GetComponent(_prevX, _prevY) = _previous;
    _prevX = ui.fixedX->value();
    _prevY = ui.fixedY->value();
  }
  _prevX = ui.fixedX->value();
  _prevY = ui.fixedY->value();
  if ( (_prevX > 0) && (_prevY > 0) )
  {
    _previous = _image.GetComponent( _prevX, _prevY  );
    HDRComponent comp;
    comp.Add(Vector4d(100,0,0,0));
    _image.GetComponent(_prevX, _prevY) = comp;
    ExposureChangedSlot(ui.sExposure->value());
  }
}

void TracingGui::LoadCamera()
{
	FileHandler handler;
	handler.Open(DEFAULT_CAMERA,"r");

	ui.xPosVal->setValue( handler.GetFloat() );
	ui.yPosVal->setValue( handler.GetFloat() );
	ui.zPosVal->setValue( handler.GetFloat() );

	ui.xDirVal->setValue(handler.GetFloat());
	ui.yDirVal->setValue(handler.GetFloat());
	ui.zDirVal->setValue(handler.GetFloat());

	ui.xzRotDeg->setValue(handler.GetInt());
	ui.fovDeg->setValue(handler.GetInt());
}

void TracingGui::SaveNewSceneSlot()
{
	UpdateSelectedModel(ui.treeViewLight->selectionModel()->selectedIndexes(),0);
	QString str = GScenes.NewSceneName();
	str.prepend(DEFAULT_PATH);
	SaveModels(str);
}

void TracingGui::UpdateExposureNumberSlot(int exposure)
{
	ui.sExposureNumber->setValue(exposure*0.1f);
}
void TracingGui::UpdateExposureSlot(double exposure)
{
	ui.sExposure->setValue(exposure*10);
}
 
void TracingGui::ShowHdr(float exponent)
{
	if (_image.W() ==0 || _image.H() == 0 )
		return;
	QSize size(_image.W(),_image.H());
	QImage qimg(size,QImage::Format_RGB32);
	ToneMapper toneMapper;
	toneMapper.SetExponent(exponent);
	for ( int i =0; i < _image.W(); i++)
	{
		for ( int j =0; j < _image.H(); j++)
		{
			HDRComponent& comp = _image.GetComponent(i,j);
      comp.SetUnit();
			int r,g,b;
			r = toneMapper.ToneMap(comp.X());
			g = toneMapper.ToneMap(comp.Y());
			b = toneMapper.ToneMap(comp.Z());
			QRgb col = qRgb(r,g,b);
			qimg.setPixel(i,j,col);
		}
	}
	ui.lImage->setPixmap(QPixmap::fromImage(qimg));
}
void TracingGui::SaveImageSlot()
{
	QString str1 = ui.filename->text();
	QByteArray ba = str1.toLocal8Bit();
	_image.SaveHdr(ba.data());
}

#include "DoubleTriangle.h"

void TracingGui::CreateFixedScene( Scene & scene)
{
	// for each model in tree, create representation
	Vector4d pos[3] = { Vector4d(1,1,0,1), Vector4d(0,1,0,1), Vector4d(1,0,0,1) };
	Geometry * plane = new DoubleTriangle(pos);
	plane->SetMaterial(CreateMaterial(Vector4d(1, 0, 0, 0)));
	Vector4d translation(ui.xModelPos->value(),ui.yModelPos->value(),ui.zModelPos->value(),0);
	translation = Vector4d(0,0,0,1) - translation;
	float angle = ui.xRotationRad->value();
	plane->Rotate(Axis_X, angle );
	angle = ui.yRotationRad->value();
	plane->Rotate(Axis_Y, angle );
	angle = ui.zRotationRad->value();
	plane->Rotate(Axis_Z, angle );
	scene.AddModel(plane);
	plane->Translate(translation);
	scene.Ambient().SetPower(Vector4d(10,10,10));
}

void TracingGui::UpdateSelectedModel( QModelIndexList indexes, int type )
{
	for ( int i =0; i < indexes.size(); i++)
	{
		GProperties& g = *_gModels->Get(indexes[i].row());
		if (type == 0)
		{
			g.position = Vector4d(ui.xModelPos->value(),ui.yModelPos->value(),ui.zModelPos->value(),0);
			g.rotation = Vector4d(ui.xRotationDeg->value(),ui.yRotationDeg->value(),ui.zRotationDeg->value());
			float radius = ui.radius->value();
      float tuberadius = ui.tuberadius->value();
			Vector4d points[] = { 
				Vector4d(ui.xVert_1->value(),ui.yVert_1->value(),ui.zVert_1->value(),1),
				Vector4d(ui.xVert_2->value(),ui.yVert_2->value(),ui.zVert_2->value(),1),
				Vector4d(ui.xVert_3->value(),ui.yVert_3->value(),ui.zVert_3->value(),1),
			};
			g.geom->SetProperty(PPoints, points);
      g.geom->SetProperty(PRadius, &radius);
      g.geom->SetProperty(PTubeRadius, &tuberadius);
		}
		if (type == 1)
		{
			g.diffuse = Vector4d(ui.dR->value(),ui.dG->value(),ui.dB->value());
			g.specular = Vector4d(ui.sR->value(),ui.sG->value(),ui.sB->value());
			g.matSpecularExp = ui.sE->value();
			g.emmisive = Vector4d(ui.eR->value(),ui.eG->value(),ui.eB->value());
		}
	}
}

void TracingGui::CreateScene( Scene & scene)
{
	UpdateSelectedModel(ui.treeView->selectionModel()->selectedIndexes(),0);
	UpdateSelectedModel(ui.treeViewLight->selectionModel()->selectedIndexes(),1);
	int i =0;
	GProperties * g;
	_scene.Clear();
  ClearMaterials();
	while ( g = _gModels->Get(i))
	{
		Geometry * geom = g->geom;
		geom->Clear();
    if ( g->emmisive.Size2() > 0 )
      geom->SetMaterial(CreateLight(g->emmisive));
    else
      geom->SetMaterial(CreateMaterial(g->diffuse, g->specular, g->matSpecularExp));
		geom->Rotate(Axis_X, toRadians( g->rotation[0] ) );
		geom->Rotate(Axis_Y, toRadians( g->rotation[1] ) );
		geom->Rotate(Axis_Z, toRadians( g->rotation[2] ) );
		Vector4d translation = Vector4d(0,0,0,1) - g->position;
		geom->Translate(translation);
		_scene.AddModel(geom);
		i++;
	}
	_scene.Ambient().SetPower(Vector4d(ui.aRValue->value(),ui.aGValue->value(),ui.aBValue->value() ) );
}

void TracingGui::Test(Camera * camera)
{
	//Vector4d res = camera->WorldToRaster(Vector4d(1,1,1,1));
	//Vector4d t1 = camera->GetRay(1,0.5).direction;
	//t1.Normalize();
	//Vector4d t2 = camera->GetRay(0,0.5).direction;
	//t2.Normalize();
	//Vector4d t3 = camera->GetRay(1,1).direction;
	//t3.Normalize();
	//float ca = t1.Dot(t2);
	//float caAngle = acos(ca);
	//float caAngle2 = toDegrees(caAngle);

//	Vector4d test = camera->WorldToRaster(Vector4d(-1.27,1.28,-1.25,1));
	Vector4d test = (Vector4d(64, 64,0,1));
	Vector4d mPos = (Vector4d(0,0,-3,1));
	Vector4d tt = camera->WorldToRaster(mPos);
	Vector4d tx = camera->RasterToWorld(test);
	Vector4d orig = camera->ModelToWorld(mPos);
	Ray ray = camera->GetRay(tt[0],tt[1]);
	Intersection isect;
	_scene.FindIntersection(ray,isect);
	Vector4d dir = tx - orig;
	Vector4d inter = orig + dir*1.28f;

	for (int i = 0; i<3; i++)
		if ( test[i] <0)
			test[i] = 0;
	Vector4d ttraster = camera->RasterToWorld(test);

}

#include "PathTraceRenderer.h"
#include "RenderThread.h"

void TracingGui::RenderSlot()
{
	bool stop = false;
	for ( int i = 0; _threads[i] && (i < MAXTHREADS); i++)
		if ( _threads[i]->isRunning() )
		{
			_threads[i]->Stop();
			stop = true;
		}
	if ( stop )
	{
    FetchResultsSlot();
		ui.bRender->setText("Render!");
		return;
	}
	ui.bRender->setText("Stop!");
	Vector4d posTranslate ( ui.xPosVal->value(), ui.yPosVal->value() , ui.zPosVal->value(), 1 );
	posTranslate =  Vector4d(0,0,0,1) - posTranslate;
	Matrix4d oMatrix;

	oMatrix.Identity();
	// originally up was 0, 1, 0
	Vector4d directionOriginal ( 0,0,1,0);
	
	Vector4d direction (ui.xDirVal->value(), ui.yDirVal->value(), ui.zDirVal->value(),0 );
	direction.Normalize();

	// find axis around which we will rotate
	Vector4d axis = directionOriginal.Cross( direction );
	axis.Normalize();
	float angle = direction.Dot(directionOriginal);
	angle = acos(angle);
	oMatrix.RotateAxis(axis,angle);

	// init scene
	CameraContext ctx;
	ctx.axis[0] = oMatrix.GetRow(2);
	ctx.axis[1] = oMatrix.GetRow(1);
	ctx.position = posTranslate;
	ctx.fov = ui.fovRad->value();
	int xDim = ui.xDim->value();
	int yDim = ui.yDim->value();

	ctx.resolution[0] = xDim;
	ctx.resolution[1] = yDim;

	_scene.Clear();
	_scene.CreateAddCamera(ctx);
	CreateScene(_scene);

	// init threads
	
  RenderContext renderCtx;
	memset(renderCtx.start,0,sizeof(renderCtx.start));
	renderCtx.scene = &_scene;
	renderCtx.end[0] = xDim;
	renderCtx.end[1] = yDim;
	renderCtx.mask = ui.directType->currentData().toInt() | ui.indirectType->currentData().toInt();

	renderCtx.renderMask = ui.rendererType->currentData().toInt();
	renderCtx.iterations = ui.iterations->value();
	renderCtx.bounces = ui.bounces->value();
  if ( ui.coordOnly->checkState() == Qt::Checked )
  {
    renderCtx.mask |= COORDS_ONLY;
    renderCtx.fixed[0] = ui.fixedX->value();
    renderCtx.fixed[1] = ui.fixedY->value();
  }
	
  for ( int i =0; i < MAXTHREADS; i++)
	{
		if (_threads[i])
			delete _threads[i];
		_threads[i] = new RenderThread(renderCtx);
		QObject::connect(_threads[i], SIGNAL(finished()), this, SLOT(FetchResultsSlot()));
		_threads[i]->start();
	}

	// convert Image to label
	// on thread finished
}

void TracingGui::FetchResultsSlot()
{
	// fetch result from each thread
  float xDim = 0;float yDim = 0;
	for ( int i =0; i < MAXTHREADS; i++)
	{
		if ( !_threads[i]->isFinished())
			return;
    _threads[i]->Saturate(xDim,yDim);
	}
  _image.SetSize(xDim,yDim);
  _image.Clear();

  for ( int i =0; i < MAXTHREADS; i++)
    _threads[i]->GetResults(_image);

  _prevX = -1;
  _prevY = -1;
	ui.sExposureNumber->setValue(0);
	ui.bRender->setText("Render!");
	ShowHdr(0);
}
TracingGui::~TracingGui()
{
  SaveSetup();
	if ( _gModels )
		delete _gModels;
}

void TracingGui::ExposureChangedSlot(int value)
{
	float e = value * 0.1f;
	ShowHdr(e);
}

void TracingGui::DeleteObjectSlot()
{
	QModelIndexList list = ui.treeView->selectionModel()->selectedIndexes();
	ui.treeView->selectionModel()->clearSelection();
	for ( int i = list.size() -1; i>=0; i--)
	{
		_gModels->Remove(list[i].row());
	}
}

#include "Point.h"

void TracingGui::AddPointSlot()
{
	Geometry * geom = new PointObject();
	QModelIndex index = _gModels->Add(geom,-1);
	ui.treeView->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
}

#include <QTCore/QDir>

void TracingGui::LoadSceneNames()
{
	// find everything that have.scene
	QStringList sceneFilter("*.scene");
	QStringList materialFilter("*.material");
	QDir directory("data/");
	QString test = directory.absolutePath();
	QStringList scenefiles = directory.entryList(sceneFilter);
	GScenes.AddScenes(scenefiles);
	QStringList materialfiles = directory.entryList(materialFilter);
	GScenes.AddMaterials(materialfiles);
}

void TracingGui::SelectionSceneChangedSlot(const QItemSelection &nSel, const QItemSelection & )
{
	if (nSel.indexes().size() == 0)
		return;
	// we care only about new selection
	if (nSel.indexes()[0].column() == 0)
	{
		ui.currentScene->setText(GScenes.data(nSel.indexes()[0],Qt::DisplayRole).toString());
		LoadModels(ui.currentScene->text());
	}
	if (nSel.indexes()[0].column() == 1)
	{
		ui.currentMaterial->setText(GScenes.data(nSel.indexes()[0],Qt::DisplayRole).toString());
		LoadMaterials(ui.currentMaterial->text());
	}
}

void TracingGui::SaveCurrentSceneSlot()
{
	UpdateSelectedModel(ui.treeView->selectionModel()->selectedIndexes(),0);
	QString sceneName = ui.currentScene->text();
	sceneName.prepend(DEFAULT_PATH);
	SaveModels(sceneName.toLocal8Bit().data());
}
void TracingGui::SaveCurrentMaterialSlot()
{
	UpdateSelectedModel(ui.treeViewLight->selectionModel()->selectedIndexes(),1);
	QString materialName = ui.currentMaterial->text();
	materialName.prepend(DEFAULT_PATH);
	SaveMaterials(materialName.toLocal8Bit().data());
}

#define SETUP_FILE "setup.txt"

#define APPLY(XX_CHECK,XX_TEXT, XX_VAL,XX_COMBO) \
  XX_TEXT(ui.currentScene)  \
  XX_TEXT(ui.currentMaterial)  \
  XX_COMBO(ui.directType)  \
  XX_VAL(ui.xPosVal)  \
  XX_VAL(ui.yPosVal)  \
  XX_VAL(ui.zPosVal)  \
  XX_VAL(ui.xDirVal)  \
  XX_VAL(ui.yDirVal)  \
  XX_VAL(ui.zDirVal)  \
  XX_VAL(ui.xzRotDeg)  \
  XX_VAL(ui.xDim)  \
  XX_VAL(ui.yDim)  \
  XX_COMBO(ui.rendererType)  \
  XX_COMBO(ui.directType)  \
  XX_COMBO(ui.indirectType)  \
  XX_VAL(ui.iterations)  \
  XX_VAL(ui.bounces)  \
  XX_CHECK(ui.coordOnly)  \
  XX_VAL(ui.fixedX)  \
  XX_VAL(ui.fixedY)  \


#define SAVE_VAL(var) {float val = var->value(); fwrite(&val,sizeof(val),1,file); }
#define SAVE_CHECK(var) { Qt::CheckState b = var->checkState(); fwrite(&b,sizeof(b),1,file); }
#define SAVE_TEXT(var) { QString s = var->text(); const QChar * c = s.data(); int len = s.size(); fwrite(&len,sizeof(int),1,file); fwrite(c,sizeof(QChar),len,file); }
#define SAVE_COMBO(var)  { int c = var->currentIndex(); fwrite(&c,sizeof(c),1,file); }

#define LOAD_VAL(var) {float ret; fread(&ret,sizeof(ret),1,file); var->setValue(ret); }
#define LOAD_CHECK(var) {Qt::CheckState b; fread(&b,sizeof(Qt::CheckState),1,file); var->setCheckState(b); }
#define LOAD_TEXT(var)  {int size; fread(&size,sizeof(int),1,file); QChar*c = new QChar[size]; fread(c,sizeof(QChar),size,file); QString str(c,size); var->setText(str); delete[] c; } 
#define LOAD_COMBO(var)  {int index; fread(&index,sizeof(int),1,file); var->setCurrentIndex(index);}
//APPLY(XX_CHECK,XX_TEXT, XX_VAL,XX_COMBO)

void TracingGui::SaveSetup()
{
  FILE *file = fopen(SETUP_FILE,"w");
  DoAssert(file);
  APPLY(SAVE_CHECK,SAVE_TEXT,SAVE_VAL,SAVE_COMBO);
  fclose(file);
}

void TracingGui::LoadSetup()
{
  FILE * file = fopen(SETUP_FILE,"r");
  if (!file)
    return;
  
  APPLY( LOAD_CHECK,LOAD_TEXT,LOAD_VAL, LOAD_COMBO )

  fclose(file);
}


QVariant SceneModels::data(const QModelIndex &index, int role) const
{
	if ( role != Qt::DisplayRole)
		return QVariant();
	if (index.column() == 0)
	{
		if (index.row() < _scenes.size())
			return _scenes[index.row()];
		return QVariant();
	}
	if (index.column() == 1)
	{
		if ( index.row() < _materials.size())
			return _materials[index.row()];
		return QVariant();
	}
	return QVariant();
}

QVariant SceneModels::headerData(int section, Qt::Orientation o, int role) const
{
	if(section == 0)
		return "Scene";
	return "Material";
}

int SceneModels::columnCount(const QModelIndex & model) const
{
	return 2;
}

int SceneModels::rowCount(const QModelIndex & model) const
{
	return qMax(_scenes.size(),_materials.size());
}

void SceneModels::AddScenes(const QStringList & list)
{
	_scenes = list;
	_materials.clear();
}

void SceneModels::AddMaterials(const QStringList & list)
{
	for ( int iList =0; iList < list.size(); iList++)
	{
		//int index = -1;
		//int pos = -1;
		//for ( int iScene = 0; iScene < _scenes.size(); iScene++ )
		//{
		//	if ( list[iList].startsWith(_scenes[iScene]) )
		//	{
		//		int len = list[iList].length();
		//		/*
		//		if ( list[iList][len] != '_')
		//			continue;
		//		len++;
		//		*/
		//		index = iScene;
		//		pos = len;
		//	}
		//}
		//if ( index >= 0)
		//  _materials[index].append(list[iList].mid(pos));
	    _materials.append(list[iList]);
	}
}

QString SceneModels::NewMaterialName() 
{
	QString name = QString("Material_%1.material").arg(_materials.size());

	QModelIndex index = SceneModels::createIndex(0,1, (void*)NULL);
	int last = qMax(_materials.size()-1,0);
	QModelIndex index2 = SceneModels::createIndex(_materials.size(),1, (void*)NULL);
	if (_materials.size() >= _scenes.size())
	{
		beginInsertRows(QModelIndex(),0,0);
		_materials.push_front(name);
		endInsertRows();
	}
	else
		_materials.push_front(name);
	emit dataChanged(index, index2);
	return name;
}

QString SceneModels::NewSceneName()
{
	QModelIndex index = SceneModels::createIndex(0,0, (void*)NULL);
	QModelIndex index2 = SceneModels::createIndex(_scenes.size(),0, (void*)NULL);
	QString name = QString("Scene_%1.scene").arg(_scenes.size());

	if (_materials.size() < _scenes.size())
	{
		beginInsertRows(QModelIndex(),0,0);
		_scenes.push_front(name);
		endInsertRows();
	}
	else
		_scenes.push_front(name);
	emit dataChanged(index, index2);
	return name;
}
