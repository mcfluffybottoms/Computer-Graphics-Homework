#include "SliderGroup.h"
#include <QLabel>
#include <cmath>
#include <qchar.h>
#include <qcheckbox.h>
#include <qcolor.h>
#include <qvector3d.h>
#include <qwidget.h>

SlidersGroup::SlidersGroup(const QString & title, std::shared_ptr<SharedData> inputData, QWidget * parent)
	: QGroupBox(title, parent), data(inputData)
{
	createSliders();
	setupLayout();
	connectSignals();
}

void SlidersGroup::createSliders()
{
	// colors
	directionLayout = new QVBoxLayout(directionGroup);
	hasDirectional_ = addToggle("Has Directional", directionLayout);
	directionalLightColor_ = addColorButton("", data->directionalLightColor, directionLayout);
	diffuse_ = addSlider(0, 100, data->diffuse, 0.01f, "Diffuse:", directionLayout);
	specular_ = addSlider(0, 100, data->specular, 0.01f, "Specular:", directionLayout);
	ambient_ = addSlider(0, 100, data->ambient, 0.01f, "Ambient:", directionLayout);

	projectionLayout = new QVBoxLayout(projectionGroup);
	hasProjection_ = addToggle("Has Projector", projectionLayout);
	//projectionLightDirection_ = addInputs("Light Direction", projectionLayout, projectionLightDir);
	projCutOff_ = addSlider(0, 3600, data->projCutOff, 0.1f, "Cutoff:", projectionLayout);
	projOuterCutOff_ = addSlider(0, 3600, data->projOuterCutOff, 0.1f, "Outer Cutoff:", projectionLayout);
	projectionLightColor_ = addColorButton("", data->projectionLightColor, projectionLayout);
	diffuseP_ = addSlider(0, 100, data->diffuseP, 0.01f, "Diffuse:", projectionLayout);
	specularP_ = addSlider(0, 100, data->specularP, 0.01f, "Specular:", projectionLayout);
	ambientP_ = addSlider(0, 100, data->ambientP, 0.01f, "Ambient:", projectionLayout);

	aoLayout = new QVBoxLayout(aoGroup);
	aoSamples_ = addSlider(0, 5, data->samples, 1.0f, "Blur Samples:", aoLayout);
	kernelSize_ = addSlider(1, 8, std::log2(data->kernelSize), 1.0f, "Kernel Size:", aoLayout);
	hasAO_ = addToggle("SSAO", aoLayout);
	data->hasAO = false;
	hasBlur_ = addToggle("Has Blur", aoLayout);
	data->hasBlur = false;
	debugAO_ = addToggle("Debug SSAO Buffer", aoLayout);
	data->debugAO = false;
	sampleRadius_ = addSlider(0, 500, data->sampleRadius, 0.01f, "Sample Radius:", aoLayout);
	setFixedWidth(300);
}

QCheckBox * SlidersGroup::addToggle(const QString & name, QVBoxLayout * layout)
{
	layout->addWidget(new QLabel(name));
	QCheckBox * widg = new QCheckBox();
	layout->addWidget(widg);
	return widg;
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
	slider->setValue((int)(current / step));
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
	this->setStyleSheet("background-color: lightblue;");
	mainLayout = new QVBoxLayout();
	mainLayout->addWidget(directionGroup);
	mainLayout->addWidget(projectionGroup);
	mainLayout->addWidget(aoGroup);
	mainLayout->setContentsMargins(10, 10, 10, 10);
	mainLayout->setSpacing(5);
	setLayout(mainLayout);
}

void SlidersGroup::connectSignals()
{
	connect(ambient_, &QSlider::valueChanged, this, &SlidersGroup::onAmbientChanged);
	connect(diffuse_, &QSlider::valueChanged, this, &SlidersGroup::onDiffuseChanged);
	connect(specular_, &QSlider::valueChanged, this, &SlidersGroup::onSpecularChanged);

	connect(ambientP_, &QSlider::valueChanged, this, &SlidersGroup::onAmbientPChanged);
	connect(diffuseP_, &QSlider::valueChanged, this, &SlidersGroup::onDiffusePChanged);
	connect(specularP_, &QSlider::valueChanged, this, &SlidersGroup::onSpecularPChanged);

	connect(directionalLightColor_, &QPushButton::clicked, this, &SlidersGroup::onColorDirButtonClicked);
	connect(projectionLightColor_, &QPushButton::clicked, this, &SlidersGroup::onColorProjButtonClicked);

	connect(hasDirectional_, &QCheckBox::clicked, this, &SlidersGroup::onHasDirectionalClicked);
	connect(hasProjection_, &QCheckBox::clicked, this, &SlidersGroup::onHasProjectionClicked);

	connect(projCutOff_, &QSlider::valueChanged, this, &SlidersGroup::onCutOffClicked);
	connect(projOuterCutOff_, &QSlider::valueChanged, this, &SlidersGroup::onOuterCutOffClicked);

	connect(aoSamples_, &QSlider::valueChanged, this, &SlidersGroup::onSamplesClicked);
	connect(kernelSize_, &QSlider::valueChanged, this, &SlidersGroup::onKernelSizeClicked);
	connect(hasAO_, &QCheckBox::clicked, this, &SlidersGroup::onHasAOClicked);
	connect(hasBlur_, &QCheckBox::clicked, this, &SlidersGroup::onHasBlurClicked);
	connect(debugAO_, &QCheckBox::clicked, this, &SlidersGroup::onDebugAOClicked);
	connect(sampleRadius_, &QSlider::valueChanged, this, &SlidersGroup::sampleRadiusClicked);
}

void SlidersGroup::onCutOffClicked(int value)
{
	data->projCutOff = value * 0.1f;
	emit cutOffClicked(value);
}

void SlidersGroup::onOuterCutOffClicked(int value)
{
	data->projOuterCutOff = value * 0.1f;
	emit outerCutOffClicked(value);
}

void SlidersGroup::onHasDirectionalClicked(bool value)
{
	data->hasDirectional = value;
	emit hasDirectionalClicked(value);
}

void SlidersGroup::onHasProjectionClicked(bool value)
{
	data->hasProjection = value;
	emit hasProjectionClicked(value);
}

void SlidersGroup::onAmbientChanged(int value)
{
	data->ambient = (float)value * 0.01f;
	emit ambientChanged(value);
}

void SlidersGroup::onDiffuseChanged(int value)
{
	data->diffuse = (float)value * 0.01f;
	emit diffuseChanged(value);
}

void SlidersGroup::onSpecularChanged(int value)
{
	data->specularP = (float)value * 0.01f;
	emit specularPChanged(value);
}

void SlidersGroup::onAmbientPChanged(int value)
{
	data->ambientP = (float)value * 0.01f;
	emit ambientPChanged(value);
}

void SlidersGroup::onDiffusePChanged(int value)
{
	data->diffuseP = (float)value * 0.01f;
	emit diffusePChanged(value);
}

void SlidersGroup::onSpecularPChanged(int value)
{
	data->specular = (float)value * 0.01f;
	emit specularChanged(value);
}

void SlidersGroup::onColorButtonClicked(QPushButton * button, QVector3D & color, const QString & title)
{
	QColor currentColor = QColor::fromRgbF(color.x(), color.y(), color.z());
	QColor newColor = QColorDialog::getColor(currentColor, this, title);
	if (newColor.isValid())
	{
		color = QVector3D(newColor.redF(), newColor.greenF(), newColor.blueF());
		updateColorButton(button, newColor);
	}
}

void SlidersGroup::onColorDirButtonClicked()
{
	onColorButtonClicked(directionalLightColor_, data->directionalLightColor, "Choose Color A");
	emit colorDirButtonClicked(data->directionalLightColor);
}

void SlidersGroup::onColorProjButtonClicked()
{
	onColorButtonClicked(projectionLightColor_, data->projectionLightColor, "Choose Color B");
	emit colorProjButtonClicked(data->projectionLightColor);
}

void SlidersGroup::onSamplesClicked(int value)
{
	data->samples = value;
	emit samplesClicked(value);
}

void SlidersGroup::onKernelSizeClicked(int value)
{
	data->kernelSize = std::pow(2, value);
	emit kernelSizeClicked(value);
}

void SlidersGroup::onHasAOClicked(bool value)
{
	data->hasAO = value;
	emit hasAOClicked(value);
}

void SlidersGroup::onHasBlurClicked(bool value)
{
	data->hasBlur = value;
	emit hasBlurClicked(value);
}

void SlidersGroup::onDebugAOClicked(bool value)
{
	data->debugAO = value;
	emit debugAOClicked(value);
}

void SlidersGroup::onSampleRadiusClicked(int value) {
	data->sampleRadius = value * 0.01;
	emit sampleRadiusClicked(value);
	
}