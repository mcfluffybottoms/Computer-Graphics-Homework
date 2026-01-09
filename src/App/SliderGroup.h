#pragma once

#include "App/SharedData.h"
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QSlider>
#include <QVector3D>
#include <memory>

class SlidersGroup : public QGroupBox
{
	Q_OBJECT

public:
	SlidersGroup(const QString & title, std::shared_ptr<SharedData> inputData, QWidget * parent = nullptr);

signals:
	void ambientChanged(int value);
	void diffuseChanged(int value);
	void specularChanged(int value);
	void ambientPChanged(int value);
	void diffusePChanged(int value);
	void specularPChanged(int value);
	void colorDirButtonClicked(const QVector3D & value);
	void colorProjButtonClicked(const QVector3D & value);
	void hasDirectionalClicked(bool value);
	void hasProjectionClicked(bool value);
	void cutOffClicked(int value);
	void outerCutOffClicked(int value);
	void samplesClicked(int value);
	void kernelSizeClicked(int value);
	void hasAOClicked(bool value);
	void hasBlurClicked(bool value);
	void debugAOClicked(bool value);
	void sampleRadiusClicked(int value);
	void biasClicked(int value);
	void sigmaClicked(int value);

private slots:
	void onAmbientChanged(int value);
	void onDiffuseChanged(int value);
	void onSpecularChanged(int value);
	void onAmbientPChanged(int value);
	void onDiffusePChanged(int value);
	void onSpecularPChanged(int value);
	void onColorDirButtonClicked();
	void onColorProjButtonClicked();

	void onHasDirectionalClicked(bool value);
	void onHasProjectionClicked(bool value);

	void onCutOffClicked(int value);
	void onOuterCutOffClicked(int value);

	void onSamplesClicked(int value);
	void onKernelSizeClicked(int value);
	void onHasAOClicked(bool value);
	void onHasBlurClicked(bool value);
	void onDebugAOClicked(bool value);
	void onSampleRadiusClicked(int value);
	void onBiasClicked(int value);
	void onSigmaClicked(int value);
private:
	void createSliders();
	void setupLayout();
	void connectSignals();

	QSlider * addSlider(int min, int max, float current, float step, const QString & name, QBoxLayout * layout) const;
	QPushButton * addColorButton(const QString & text, const QVector3D & color, QBoxLayout * layout) const;
	QCheckBox * addToggle(const QString & name, QVBoxLayout * layout);

	void updateColorButton(QPushButton * button, const QColor & color) const;
	void onColorButtonClicked(QPushButton * button, QVector3D & color, const QString & title);

	QColorDialog * colorPicker_;

	std::shared_ptr<SharedData> data;

	// colors
	QSlider * ambient_;
	QSlider * diffuse_;
	QSlider * specular_;

	QSlider * ambientP_;
	QSlider * diffuseP_;
	QSlider * specularP_;

	QPushButton * directionalLightColor_;
	QPushButton * projectionLightColor_;

	QSlider * projCutOff_;
	QSlider * projOuterCutOff_;

	QCheckBox * hasDirectional_;
	QCheckBox * hasProjection_;

	// AO
	QSlider * aoSamples_;
	QSlider * kernelSize_;
	QSlider * sampleRadius_;
	QCheckBox * hasAO_;
	QCheckBox * hasBlur_;
	QCheckBox * debugAO_;
	QSlider * sigma_;
	QSlider * bias_;

	// layouts
	QGroupBox * directionGroup = new QGroupBox("Direction Light Parameters");
	QGroupBox * projectionGroup = new QGroupBox("Projection Light Parameters");
	QGroupBox * aoGroup = new QGroupBox("Ambient Occlusion Parameters");

	QVBoxLayout * lightLayout;
	QVBoxLayout * directionLayout;
	QVBoxLayout * projectionLayout;
	QVBoxLayout * aoLayout;

	QLayout * mainLayout;
};