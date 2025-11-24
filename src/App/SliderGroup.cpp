#include "SliderGroup.h"
#include <QLabel>
#include <new>
#include <qchar.h>
#include <qcheckbox.h>
#include <qcolor.h>
#include <qvector3d.h>
#include <qwidget.h>

SlidersGroup::SlidersGroup(const QString & title, QWidget * parent)
	: QGroupBox(title, parent)
{
	createSliders();
	setupLayout();
	connectSignals();
}

QVector3D SlidersGroup::getVector(int vec_name) {
	if(vec_name == 1) {
		return directionalLightPosition_->value();
	} else if (vec_name == 2) {
		return projectionLightPosition_->value();
	} else if (vec_name == 3) {
		return projectionLightDirection_->value();
	}
	return {0, 0, 0};
}

void SlidersGroup::createSliders()
{

	// positions
	lightLayout = new QVBoxLayout(lightGroup);
	diffuse_ = addSlider(0, 100, diffuse, 0.01f, "Diffuse:", lightLayout);
	specular_ = addSlider(0, 100, specular, 0.01f, "Specular:", lightLayout);
	ambient_ = addSlider(0, 100, ambient, 0.01f, "Ambient:", lightLayout);

	// colors
	directionLayout = new QVBoxLayout(directionGroup);
	hasDirectional_ = addToggle("Has Directional", directionLayout);
	directionalLightPosition_ = addInputs("Light Position", directionLayout);
	directionalLightColor_ = addColorButton("", directionalLightColor, directionLayout);
	
	projectionLayout = new QVBoxLayout(projectionGroup);
	hasProjection_ = addToggle("Has Projector", projectionLayout);
	projectionLightDirection_ = addInputs("Light Direction", projectionLayout);
	projectionLightPosition_ = addInputs("Light Position", projectionLayout);
	projCutOff_ = addSlider(0, 3600, projCutOff, 0.1f, "Cutoff:", projectionLayout);
	projOuterCutOff_ = addSlider(0, 3600, projOuterCutOff, 0.1f, "Outer Cutoff:", projectionLayout);
	projectionLightColor_ = addColorButton("", projectionLightColor, projectionLayout);
	
	
	setFixedWidth(300);
}

Vector3DInputWidget* SlidersGroup::addInputs(const QString & name, QVBoxLayout * layout) {
	layout->addWidget(new QLabel(name));
	Vector3DInputWidget* widg = new Vector3DInputWidget();
	layout->addWidget(widg);
	return widg;
}

QCheckBox* SlidersGroup::addToggle(const QString & name, QVBoxLayout * layout) {
	layout->addWidget(new QLabel(name));
	QCheckBox* widg = new QCheckBox();
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
	mainLayout->addWidget(lightGroup);
	mainLayout->addWidget(directionGroup);
	mainLayout->addWidget(projectionGroup);
	mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);
	setLayout(mainLayout);
}

void SlidersGroup::connectSignals()
{
	connect(ambient_, &QSlider::valueChanged, this, &SlidersGroup::onAmbientChanged);
	connect(diffuse_, &QSlider::valueChanged, this, &SlidersGroup::onDiffuseChanged);
	connect(specular_, &QSlider::valueChanged, this, &SlidersGroup::onSpecularChanged);
	connect(directionalLightColor_, &QPushButton::clicked, this, &SlidersGroup::onColorDirButtonClicked);
	connect(projectionLightColor_, &QPushButton::clicked, this, &SlidersGroup::onColorProjButtonClicked);

	connect(hasDirectional_, &QCheckBox::clicked, this, &SlidersGroup::onHasDirectionalClicked);
	connect(hasProjection_, &QCheckBox::clicked, this, &SlidersGroup::onHasProjectionClicked);

	connect(projCutOff_, &QSlider::valueChanged, this, &SlidersGroup::onCutOffClicked);
	connect(projOuterCutOff_, &QSlider::valueChanged, this, &SlidersGroup::onOuterCutOffClicked);

}

void SlidersGroup::onCutOffClicked(int value) {
	projCutOff = value * 0.1f;
	emit cutOffClicked(value);
}

void SlidersGroup::onOuterCutOffClicked(int value) {
	projOuterCutOff = value * 0.1f;
	emit outerCutOffClicked(value);
}

void SlidersGroup::onHasDirectionalClicked(bool value) {
	hasDirectional = value;
	emit hasDirectionalClicked(value);
}

void SlidersGroup::onHasProjectionClicked(bool value) {
	hasProjection = value;
	emit hasProjectionClicked(value);
}

void SlidersGroup::onAmbientChanged(int value)
{
	ambient = (float)value * 0.01f;
	emit ambientChanged(value);
}

void SlidersGroup::onDiffuseChanged(int value)
{
	diffuse = (float)value * 0.01f;
	emit diffuseChanged(value);
}

void SlidersGroup::onSpecularChanged(int value)
{
	specular = (float)value * 0.01f;
	emit specularChanged(value);
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

void SlidersGroup::onColorDirButtonClicked()
{
    onColorButtonClicked(directionalLightColor_, directionalLightColor, "Choose Color A");
    emit colorDirButtonClicked(directionalLightColor);
}

void SlidersGroup::onColorProjButtonClicked()
{
    onColorButtonClicked(projectionLightColor_, projectionLightColor, "Choose Color B");
    emit colorProjButtonClicked(projectionLightColor);
}