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
	Vector4d matParams;
	MaterialType material;
	Geometry * geom;
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
	void AddTriangleSlot();
	void LoadSceneSlot();
	void DeleteObjectSlot();

private:
	Ui::TracingGuiClass ui;
	void ShowHdr(float exponent);
	void CreateScene( Scene & scene);	
	void CreateFixedScene( Scene & scene);
	void UpdateSelectedModel( QModelIndexList indexes );
	void SaveModels( );
	void SaveCamera();
	void LoadCamera();
	void LoadModels();
	void Test(Camera * camera);
};

#endif // TRACINGGUI_H
