#include "DoubleIntSpinBox.h"

void DoubleIntSpinBox::setValue(double val)
{
	QDoubleSpinBox::setValue(val);
}

void DoubleIntSpinBox::setValue(int val)
{
	double d = 0;
	if ( _type == 0)
	{
		d = toRadians(val);
	}
	else
	{
		d = val*0.1f;
	}
	QDoubleSpinBox::setValue(d);
}

int DoubleIntSpinBox::convertorType() const
{
	return _type;
}

void DoubleIntSpinBox::setConvertorType(int val)
{
	_type = val;
}

DoubleIntSpinBox::DoubleIntSpinBox(QWidget * parent) : QDoubleSpinBox(parent)
{
	_type = -1;
}

