#include <QAbstractListModel>

#include "tracinggui.h"
#include "TrackProgress.h"
#include "Camera.h"
#include "scene.h"
#include "testRenderer.h"
#include "Plane.h"
#include "AmbientLight.h"
#include <qpainter.h>
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
	if (role == Qt::DisplayRole)
	{
		int rindex = 0;
		int type = _geometries[index.row()].geom->Type();
		for (int i =0; i< index.row(); i++)
		{
			if ( type == _geometries[i].geom->Type())
				rindex++;
		}
		return QString("%1_%2").arg(typeNames[type]).arg(rindex);
		//return QString("Sphere%1").arg(index.row() + 1);
	}
	return QVariant();
}

QModelIndex GModelObjects::Add(Geometry * geom)
{
	beginInsertRows(QModelIndex(), _geometries.size(), _geometries.size() + 1);
	GProperties prop;
	memset(&prop, 0, sizeof(prop));
	prop.position = Vector4d(0,0,3,0);
	prop.matParams = Vector4d(1,1,1,1);
	prop.geom = geom;
	_geometries.push_back(prop);
	endInsertRows();
	return createIndex(_geometries.size()-1,0);
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

TracingGui::TracingGui(QWidget *parent)
	: QMainWindow(parent), _image(128,128),_gModels(NULL)
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

	// connect buttons to create types
	connect(ui.exportScene, SIGNAL(clicked(void)),this,SLOT(SaveSceneSlot()));
	connect(ui.loadScene, SIGNAL(clicked(void)),this,SLOT(LoadSceneSlot()));

	// connect button to create export/import
	connect( ui.addSphere, SIGNAL(clicked(void)), this, SLOT(AddSphereSlot()));
	connect( ui.addTriangle, SIGNAL(clicked(void)), this, SLOT(AddTriangleSlot()));
	connect( ui.delObject, SIGNAL(clicked(void)), this, SLOT(DeleteObjectSlot()));

	_gModels = new GModelObjects(NULL);
	ui.treeView->setModel(_gModels);
	//selection changes shall trigger a slot
	QItemSelectionModel *selectionModel= ui.treeView->selectionModel();
	connect(selectionModel, SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
		this, SLOT(SelectionModelChangedSlot(const QItemSelection &, const QItemSelection &)));
	ui.rendererType->addItem("TestRenderer",QVariant(0));
	ui.rendererType->addItem("PathtracerRenderer",QVariant(1));
}

#include "Sphere.h"

void TracingGui::SelectionModelChangedSlot(const QItemSelection & newSel, const QItemSelection &oldSel)
{
	UpdateSelectedModel(oldSel.indexes());
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
	//ui.MaterialType 
	ui.cr->setValue(gProp.matParams[0]*255);
	ui.cg->setValue(gProp.matParams[1]*255);
	ui.cb->setValue(gProp.matParams[2]*255);
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
void TracingGui::AddSphereSlot()
{
	Geometry * geom = new Sphere(1);
	QModelIndex index = _gModels->Add(geom);
	ui.treeView->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
}

#include "doubleTriangle.h"

void TracingGui::AddTriangleSlot()
{
	Geometry * geom = new DoubleTriangle(NULL);
	QModelIndex index = _gModels->Add(geom);
	ui.treeView->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
}

#include "FileHandler.h"
#define DEFAULT_SCENE "../../data/test.scene"
#define DEFAULT_CAMERA "../../data/camera.scene"

void TracingGui::LoadModels()
{
	ui.treeView->selectionModel()->clear();
	_gModels->Clear();
	FileHandler handler;
	handler.Open(DEFAULT_SCENE,"r");
	while (true)
	{
		GProperties * prop;
		int w;
		handler.Read(&w,sizeof(int),1);
		if (handler.End())
			break;
		Geometry * geom;
		bool points = false;
		if ( w == TypeSphere )
			geom = new Sphere(1);
		else if (w == TypeTriangle)
		{
			geom = new DoubleTriangle(NULL);
			points = true;
		}
		else
		{
			DoAssert(false);
		}
		QModelIndex index = _gModels->Add(geom);
		GProperties * g = _gModels->Get(index.row());

		handler.Read( &g->material,sizeof (g->material), 1 );
		handler.Read( &g->matParams, sizeof (g->matParams), 1);
		handler.Read( &g->position, sizeof (g->position), 1 );
		handler.Read( &g->rotation, sizeof (g->rotation), 1);
		if ( points )
		{
			Vector4d test[3];
			handler.Read(&test,sizeof(Vector4d),3);		
			geom->SetProperty(PPoints,test);
		}
	}
}

void TracingGui::LoadSceneSlot()
{
	LoadModels();
	LoadCamera();
}

void TracingGui::SaveModels( )
{
	UpdateSelectedModel(ui.treeView->selectionModel()->selectedIndexes());

	FileHandler handler;
	handler.Open(DEFAULT_SCENE,"w");

	int i =0;
	GProperties * g;
	while ( g = _gModels->Get(i))
	{
		GProperties * prop;
		Geometry * geom = g->geom;

		int tp = geom->Type();
		handler.Write(&tp,1,sizeof(int));

		GProperties * g = _gModels->Get(i);

		handler.Write( &g->material, sizeof (g->material),1 );
		handler.Write( &g->matParams, sizeof (g->matParams),1 );
		handler.Write( &g->position, sizeof (g->position),1 );
		handler.Write( &g->rotation, sizeof (g->rotation),1 );

		Vector4d * test = (Vector4d *) geom->GetProperty(PPoints);
		if (test)
			handler.Write(test,sizeof(Vector4d),3);	
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

void TracingGui::SaveSceneSlot()
{
	// saveModels
	SaveModels();

	SaveCamera();

	//SaveLights()
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
			HDRComponent comp = _image.GetComponent(i,j);
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
	plane->SetMaterial(CreateMaterial(MDiffuse, Vector4d(1,0,0,0)));
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

void TracingGui::UpdateSelectedModel( QModelIndexList indexes )
{
	for ( int i =0; i < indexes.size(); i++)
	{
		GProperties& g = *_gModels->Get(indexes[i].row());
		g.matParams = Vector4d(ui.cr->value()/255.f,ui.cg->value()/255.f,ui.cb->value()/255.f);
		g.position = Vector4d(ui.xModelPos->value(),ui.yModelPos->value(),ui.zModelPos->value(),0);
		g.rotation = Vector4d(ui.xRotationDeg->value(),ui.yRotationDeg->value(),ui.zRotationDeg->value());
		Vector4d points[] = { 
			Vector4d(ui.xVert_1->value(),ui.yVert_1->value(),ui.zVert_1->value(),1),
			Vector4d(ui.xVert_2->value(),ui.yVert_2->value(),ui.zVert_2->value(),1),
			Vector4d(ui.xVert_3->value(),ui.yVert_3->value(),ui.zVert_3->value(),1),
		};
		g.geom->SetProperty(PPoints, points);
	}
}

void TracingGui::CreateScene( Scene & scene)
{
	UpdateSelectedModel(ui.treeView->selectionModel()->selectedIndexes());
	int i =0;
	GProperties * g;
	while ( g = _gModels->Get(i))
	{
		Geometry * geom = g->geom;
		geom->Clear();
		geom->SetMaterial( CreateMaterial(MDiffuse, g->matParams) );
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

void TracingGui::RenderSlot()
{
	int xDim = ui.xDim->value();
	int yDim = ui.yDim->value();
	_image.Clear();
	_image.SetSize(xDim,yDim);

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
	
	float fov = ui.fovRad->value();
	Camera * camera = Camera::CreateCamera(posTranslate, oMatrix.GetRow(2), oMatrix.GetRow(1) ,fov);

	camera->SetResolution(ui.xDim->value(),ui.yDim->value());
		
	_scene.Clear();
	CreateScene(_scene);
	
	Test(camera);

	QVariant val = ui.rendererType->currentData();
	Renderer * renderer;
	switch (val.toInt())
	{
	case 0:
		renderer=new TestRenderer();
		break;
	case 1:
		renderer = new PathTraceRenderer();
		break;
	default:
		throw "renderer has unexpected values";
	}
	renderer->Init(&_scene,&_image,camera);
	renderer->Render();
	// convert Image to label
	ShowHdr(0);
}

TracingGui::~TracingGui()
{
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
