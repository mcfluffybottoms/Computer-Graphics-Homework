#include "SliderGroup.h"
#include <QLabel>
#include <qchar.h>
#include <qcolor.h>
#include <qvector3d.h>

SlidersGroup::SlidersGroup(const QString & title, QWidget * parent)
	: QGroupBox(title, parent)
{
	createSliders();
	setupLayout();
	connectSignals();
}

void SlidersGroup::createSliders()
{

	// positions
	positionsLayout = new QVBoxLayout(positionsGroup);
	THRESHOLD1_ = addSlider(-1000, 1000, fromX, 0.1f, "THRESHOLD1:", positionsLayout);
	THRESHOLD2_ = addSlider(-10000, 10000, fromY, 0.1f, "THRESHOLD2:", positionsLayout);
    fromX_ = addSlider(-50, 50, fromX, 0.1f, "From X:", positionsLayout);
	fromY_ = addSlider(-50, 50, fromY, 0.1f, "From Y:", positionsLayout);
	size_ = addSlider(1, 2000, sizeX, 1, "Size X:", positionsLayout);
	maxIters_ = addSlider(1, 800, (float)maxIters, 1, "Max Iterations:", positionsLayout);
	// colors
	colorsLayout = new QVBoxLayout(colorsGroup);
	brightness_ = addSlider(0, 30, brightness, 0.1f, "Brightness:", colorsLayout);
	contrast_ = addSlider(0, 30, contrast, 0.1f, "Contrast:", colorsLayout);
	colorA_ = addColorButton("", colorA, colorsLayout);
	colorB_ = addColorButton("", colorB, colorsLayout);
	colorC_ = addColorButton("", colorC, colorsLayout);
	colorD_ = addColorButton("", colorD, colorsLayout);
	setFixedWidth(200);
}

QSlider * SlidersGroup::addSlider(
	int min,
	int max,
	float current,
	float step,
	const QString & name,
	QBoxLayout * layout) const
{
	QSlider * slider = new QSlider(Qt::Horizontal);
	slider->setRange(min, max);
	slider->setValue((int)(current * step));
	layout->addWidget(new QLabel(name));
	layout->addWidget(slider);
	return slider;
}

QPushButton * SlidersGroup::addColorButton(const QString & text, const QVector3D & color, QBoxLayout * layout) const
{
	QPushButton * button = new QPushButton(text);
	button->setFixedHeight(30);
	updateColorButton(button, QColor::fromRgbF(color.x(), color.y(), color.z()));
	layout->addWidget(button);
	return button;
}

void SlidersGroup::updateColorButton(QPushButton * button, const QColor & color) const
{
	button->setStyleSheet(QString("background-color: %1;").arg(color.name()));
}

void SlidersGroup::setupLayout()
{
	mainLayout = new QVBoxLayout();
	mainLayout->addWidget(positionsGroup);
	mainLayout->addWidget(colorsGroup);
	setLayout(mainLayout);
}

void SlidersGroup::connectSignals()
{
	connect(fromX_, &QSlider::valueChanged, this, &SlidersGroup::onFromXChanged);
	connect(fromY_, &QSlider::valueChanged, this, &SlidersGroup::onFromYChanged);
	connect(size_, &QSlider::valueChanged, this, &SlidersGroup::onSizeChanged);
	connect(maxIters_, &QSlider::valueChanged, this, &SlidersGroup::onMaxItersChanged);

    connect(THRESHOLD1_, &QSlider::valueChanged, this, &SlidersGroup::onTHRESHOLD1Changed);
    connect(THRESHOLD2_, &QSlider::valueChanged, this, &SlidersGroup::onTHRESHOLD2Changed);

	connect(brightness_, &QSlider::valueChanged, this, &SlidersGroup::onBrightnessChanged);
	connect(contrast_, &QSlider::valueChanged, this, &SlidersGroup::onContrastChanged);
	connect(colorA_, &QPushButton::clicked, this, &SlidersGroup::onColorAButtonClicked);
	connect(colorB_, &QPushButton::clicked, this, &SlidersGroup::onColorBButtonClicked);
	connect(colorC_, &QPushButton::clicked, this, &SlidersGroup::onColorCButtonClicked);
	connect(colorD_, &QPushButton::clicked, this, &SlidersGroup::onColorDButtonClicked);
}

void SlidersGroup::onFromXChanged(int value)
{
	fromX = (float)value * 0.1f;
	emit fromXChanged(value);
}

void SlidersGroup::onFromYChanged(int value)
{
	fromY = (float)value * 0.1f;
	emit fromYChanged(value);
}

void SlidersGroup::onSizeChanged(int value)
{
	wid = (float)value;
    hei = wid;
	emit sizeChanged(value);
}

void SlidersGroup::onMaxItersChanged(int value)
{
	maxIters = value;
	emit maxItersChanged(value);
}

void SlidersGroup::onBrightnessChanged(int value)
{
	brightness = (float)value * 0.1f;
	emit brightnessChanged(value);
}

void SlidersGroup::onContrastChanged(int value)
{
	contrast = (float)value * 0.1f;
	emit contrastChanged(value);
}

void SlidersGroup::onTHRESHOLD1Changed(int value)
{
	THRESHOLD1 = (float)value;
	emit THRESHOLD1Changed(value);
}

void SlidersGroup::onTHRESHOLD2Changed(int value)
{
	THRESHOLD2 = (float)value;
	emit THRESHOLD2Changed(value);
}

void SlidersGroup::onColorButtonClicked(QPushButton * button, QVector3D& color, const QString& title)
{
	QColor currentColor = QColor::fromRgbF(color.x(), color.y(), color.z());
	QColor newColor = QColorDialog::getColor(currentColor, this, title);
	if (newColor.isValid())
	{
		color = QVector3D(newColor.redF(), newColor.greenF(), newColor.blueF());
		updateColorButton(button, newColor);
	}
}

void SlidersGroup::onColorAButtonClicked()
{
    onColorButtonClicked(colorA_, colorA, "Choose Color A");
    emit colorAChanged(colorA);
}

void SlidersGroup::onColorBButtonClicked()
{
    onColorButtonClicked(colorB_, colorB, "Choose Color B");
    emit colorAChanged(colorB);
}

void SlidersGroup::onColorCButtonClicked()
{
    onColorButtonClicked(colorC_, colorC, "Choose Color C");
    emit colorAChanged(colorC);
}

void SlidersGroup::onColorDButtonClicked()
{
    onColorButtonClicked(colorD_, colorD, "Choose Color D");
    emit colorAChanged(colorD);
}