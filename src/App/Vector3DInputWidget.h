#pragma once

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector3D>
#include <QWidget>

class Vector3DInputWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QVector3D value READ value WRITE setValue NOTIFY valueChanged)

public:
	explicit Vector3DInputWidget(QWidget * parent = nullptr);

	QVector3D value() const;

	void setRange(double min, double max);
	void setDecimals(int decimals);
	void setSingleStep(double step);
	void setSuffix(const QString & suffix);

public slots:
	void setValue(const QVector3D & value);
	void setValue(double x, double y, double z);

signals:
	void valueChanged(const QVector3D & value);

private slots:
	void onXChanged(double value);
	void onYChanged(double value);
	void onZChanged(double value);

private:
	QDoubleSpinBox * spinBoxX_;
	QDoubleSpinBox * spinBoxY_;
	QDoubleSpinBox * spinBoxZ_;

	///QVector3D vec{0, 0, 0};
};