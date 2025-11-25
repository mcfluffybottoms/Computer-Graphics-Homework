
#include "Vector3DInputWidget.h"

Vector3DInputWidget::Vector3DInputWidget(QWidget * parent)
	: QWidget(parent)
{
	QHBoxLayout * layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	// X component
	layout->addWidget(new QLabel("X:"));
	spinBoxX_ = new QDoubleSpinBox();
	spinBoxX_->setRange(-1000.0, 1000.0);
	spinBoxX_->setDecimals(3);
	spinBoxX_->setSingleStep(0.1);
	layout->addWidget(spinBoxX_);

	// Y component
	layout->addWidget(new QLabel("Y:"));
	spinBoxY_ = new QDoubleSpinBox();
	spinBoxY_->setRange(-1000.0, 1000.0);
	spinBoxY_->setDecimals(3);
	spinBoxY_->setSingleStep(0.1);
	layout->addWidget(spinBoxY_);

	// Z component
	layout->addWidget(new QLabel("Z:"));
	spinBoxZ_ = new QDoubleSpinBox();
	spinBoxZ_->setRange(-1000.0, 1000.0);
	spinBoxZ_->setDecimals(3);
	spinBoxZ_->setSingleStep(0.1);
	layout->addWidget(spinBoxZ_);

	// Connections
	connect(spinBoxX_, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			this, &Vector3DInputWidget::onXChanged);
	connect(spinBoxY_, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			this, &Vector3DInputWidget::onYChanged);
	connect(spinBoxZ_, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			this, &Vector3DInputWidget::onZChanged);
}

QVector3D Vector3DInputWidget::value() const
{
	return QVector3D(
		static_cast<float>(spinBoxX_->value()),
		static_cast<float>(spinBoxY_->value()),
		static_cast<float>(spinBoxZ_->value()));
}

void Vector3DInputWidget::setValue(const QVector3D & value)
{
	spinBoxX_->blockSignals(true);
	spinBoxY_->blockSignals(true);
	spinBoxZ_->blockSignals(true);

	spinBoxX_->setValue(value.x());
	spinBoxY_->setValue(value.y());
	spinBoxZ_->setValue(value.z());

	spinBoxX_->blockSignals(false);
	spinBoxY_->blockSignals(false);
	spinBoxZ_->blockSignals(false);
}

void Vector3DInputWidget::setValue(double x, double y, double z)
{
	setValue(QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
}

void Vector3DInputWidget::setRange(double min, double max)
{
	spinBoxX_->setRange(min, max);
	spinBoxY_->setRange(min, max);
	spinBoxZ_->setRange(min, max);
}

void Vector3DInputWidget::setDecimals(int decimals)
{
	spinBoxX_->setDecimals(decimals);
	spinBoxY_->setDecimals(decimals);
	spinBoxZ_->setDecimals(decimals);
}

void Vector3DInputWidget::setSingleStep(double step)
{
	spinBoxX_->setSingleStep(step);
	spinBoxY_->setSingleStep(step);
	spinBoxZ_->setSingleStep(step);
}

void Vector3DInputWidget::setSuffix(const QString & suffix)
{
	spinBoxX_->setSuffix(suffix);
	spinBoxY_->setSuffix(suffix);
	spinBoxZ_->setSuffix(suffix);
}

void Vector3DInputWidget::onXChanged(double)
{
	///vec.setX(value);
	emit valueChanged(this->value());
}

void Vector3DInputWidget::onYChanged(double)
{
	emit valueChanged(this->value());
}

void Vector3DInputWidget::onZChanged(double)
{
	emit valueChanged(this->value());
}