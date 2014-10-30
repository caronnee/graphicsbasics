#ifndef TRACINGGUI_H
#define TRACINGGUI_H

#include <QtWidgets/QMainWindow>
#include "../ui_tracinggui.h"
#include "image.h"

#include "scene.h"

#include <QStandardItemModel>
#include <QStandardItem>

struct GProperties
{
	Vector4d rotation;
	Vector4d position;
	// material color
	Vector4d matDiffuse;
	Vector4d matSpecular;
	Vector4d matEmmisive;
	float matSpecularExp;
	MaterialType material;
	Geometry * geom;
	QString name;
};

class GModelObjects: public QAbstractListModel
{
	Q_OBJECT

	QList <GProperties> _geometries;
public:
	GModelObjects(QObject *parent);
	int rowCount(const QModelIndex & /*parent*/) const;
	int columnCount(const QModelIndex & /*parent*/) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation o, int role) const;
	QModelIndex Add(Geometry * geom);
	GProperties * Get(int row);
	void Clear();
	void Remove(int index);
	Qt::ItemFlags flags(const QModelIndex & /*index*/) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role);
};

class SceneModels : public QAbstractListModel
{
	Q_OBJECT

	QStringList _scenes;
	QStringList _materials;
public:
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation o, int role) const;

	int columnCount(const QModelIndex &) const;
	int rowCount(const QModelIndex &) const;
	void AddScenes(const QStringList & list);
	void AddMaterials(const QStringList & list);
	QString NewMaterialName();
};

class TracingGui : public QMainWindow
{
	Q_OBJECT

	Image _image;
	GModelObjects * _gModels;
	Scene _scene;

public:
	TracingGui(QWidget *parent = 0);
	~TracingGui();

private slots:
	void RenderSlot();
	void SaveImageSlot();
	void ExposureChangedSlot(int value);
	void UpdateExposureSlot(double exposure);
	void UpdateExposureNumberSlot(int exposure);
	void SaveSceneSlot();
	void AddSphereSlot();
	void SelectionModelChangedSlot(const QItemSelection & newSel, const QItemSelection &oldSel);
	void SelectionMaterialChangedSlot(const QItemSelection & newSel, const QItemSelection &oldSel);
	void AddTriangleSlot();
	void LoadSceneSlot();
	void DeleteObjectSlot();
	void AddPointSlot();
	void SaveNewSceneSlot();
	void SaveMaterialSlot();
	void SelectionSceneChangedSlot(const QItemSelection &, const QItemSelection &);

private:
	Ui::TracingGuiClass ui;
	void ShowHdr(float exponent);
	void CreateScene( Scene & scene);	
	void CreateFixedScene( Scene & scene);
	void UpdateSelectedModel( QModelIndexList indexes, int type );
	void SaveModels( );
	void SaveCamera();
	void LoadCamera();
	void LoadModels();
	void Test(Camera * camera);
	void CreateMaterialName();
	void CreateSceneName();
	void LoadSceneNames();
};

#endif // TRACINGGUI_H
