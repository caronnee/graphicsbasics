#ifndef __DOUBLEINTSPINBOX__
#define __DOUBLEINTSPINBOX__

#include <QTCore/qobject.h>
#include <QtWidgets/QDoubleSpinBox>
#include "MathUtil.h"

class DoubleIntSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

	//Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
	Q_PROPERTY(int ConvertorType READ convertorType WRITE setConvertorType )

	int _type;
public:
	DoubleIntSpinBox(QWidget * parent = NULL);
public:
	int convertorType() const;
	void setConvertorType(int val);

public slots:
	void setValue(int val);
	void setValue(double val);
};

#endif