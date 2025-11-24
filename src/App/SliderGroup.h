#pragma once
#include "App/Vector3DInputWidget.h"
#include <QBoxLayout>
#include <QColorDialog>
#include <QGroupBox>
#include <QSlider>
#include <QPushButton>
#include <QColor>
#include <QVector3D>
#include <QCheckBox>

class SlidersGroup : public QGroupBox
{
	Q_OBJECT

public:
	SlidersGroup(const QString & title, QWidget * parent = nullptr);

    // lights
    float ambient = 1.0f;
	float diffuse = 1.0f;
    float specular = 1.0f;
    QVector3D directionalLightColor = QVector3D(0.5f, 0.5f, 0.5f);
    QVector3D projectionLightColor = QVector3D(0.5f, 0.5f, 0.5f);

    QVector3D directionalLightPosition = QVector3D(0.5f, 0.5f, 0.5f);
    QVector3D projectionLightPosition = QVector3D(0.5f, 0.5f, 0.5f);

    QVector3D getVector(int vec_name);

    QVector3D projectionLightDir = QVector3D(0.5f, 0.5f, 0.5f);
    
    float projCutOff{20.f};
	float projOuterCutOff{50.0f};

    bool hasDirectional = false;
    bool hasProjection = false;

signals:
    void ambientChanged(int value);
	void diffuseChanged(int value);
    void specularChanged(int value);
    void colorDirButtonClicked(const QVector3D& value);
    void colorProjButtonClicked(const QVector3D& value);
    void hasDirectionalClicked(bool value);
    void hasProjectionClicked(bool value);
    void cutOffClicked(int value);
    void outerCutOffClicked(int value);

private slots:
    void onAmbientChanged(int value);
	void onDiffuseChanged(int value);
    void onSpecularChanged(int value);
    void onColorDirButtonClicked();
    void onColorProjButtonClicked();

    void onHasDirectionalClicked(bool value);
    void onHasProjectionClicked(bool value);

    void onCutOffClicked(int value);
    void onOuterCutOffClicked(int value);
private:
	void createSliders();
	void setupLayout();
	void connectSignals();

    QSlider * addSlider(int min, int max, float current, float step, const QString& name, QBoxLayout* layout) const;
    QPushButton * addColorButton(const QString & text, const QVector3D & color, QBoxLayout * layout) const;
    Vector3DInputWidget* addInputs(const QString & name, QVBoxLayout * layout);
    QCheckBox* addToggle(const QString & name, QVBoxLayout * layout);
    
    void updateColorButton(QPushButton * button, const QColor & color) const;
    void onColorButtonClicked(QPushButton * button, QVector3D& color, const QString& title);

    QColorDialog * colorPicker_;

    // colors
    QSlider * ambient_;
	QSlider * diffuse_;
    QSlider * specular_;
    QPushButton * directionalLightColor_;
	QPushButton * projectionLightColor_;
    Vector3DInputWidget* directionalLightPosition_;
    Vector3DInputWidget* projectionLightPosition_;
    Vector3DInputWidget* projectionLightDirection_;

    QSlider * projCutOff_;
	QSlider * projOuterCutOff_;

    QCheckBox * hasDirectional_;
    QCheckBox * hasProjection_;

    // layouts
    QGroupBox* lightGroup = new QGroupBox("Light Parameters");
    QGroupBox* directionGroup = new QGroupBox("Direction Light Parameters");
    QGroupBox* projectionGroup = new QGroupBox("Projection Light Parameters");
    QVBoxLayout * lightLayout;
    QVBoxLayout * directionLayout;
    QVBoxLayout * projectionLayout;
    QLayout * mainLayout;
};