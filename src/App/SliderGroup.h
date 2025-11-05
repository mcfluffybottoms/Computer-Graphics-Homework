#pragma once
#include <QBoxLayout>
#include <QColorDialog>
#include <QGroupBox>
#include <QSlider>
#include <QPushButton>
#include <QColor>
#include <QVector3D>

class SlidersGroup : public QGroupBox
{
	Q_OBJECT

public:
	SlidersGroup(const QString & title, QWidget * parent = nullptr);
	// positions
    float wid = 1000.0f;
	float hei = 1000.0f;
	float fromX = -1.5f;
	float fromY = -1.5f;
	float sizeX = 1000.0f;
	float sizeY = 1000.0f;
	int maxIters = 100;
	bool isSmoothing = true;

    // colors
    float brightness = 1.0f;
	float contrast = 1.0f;
    QVector3D colorA = QVector3D(0.5f, 0.5f, 0.5f);
    QVector3D colorB = QVector3D(0.5f, 0.5f, 0.5f);
    QVector3D colorC = QVector3D(1.0f, 1.0f, 1.0f);
    QVector3D colorD = QVector3D(0.3f, 0.2f, 0.2f);

signals:
	void fromXChanged(int value);
	void fromYChanged(int value);
	void sizeChanged(int value);
	void maxItersChanged(int value);

    void brightnessChanged(int value);
	void contrastChanged(int value);
    void colorAChanged(const QVector3D& value);
    void colorBChanged(const QVector3D& value);
    void colorCChanged(const QVector3D& value);
    void colorDChanged(const QVector3D& value);

private slots:
	void onFromXChanged(int value);
	void onFromYChanged(int value);
	void onSizeChanged(int value);
	void onMaxItersChanged(int value);

    void onBrightnessChanged(int value);
    void onContrastChanged(int value);
    void onColorAButtonClicked();
    void onColorBButtonClicked();
    void onColorCButtonClicked();
    void onColorDButtonClicked();
private:
	void createSliders();
	void setupLayout();
	void connectSignals();

    QSlider * addSlider(int min, int max, float current, float step, const QString& name, QBoxLayout* layout) const;
    QPushButton * addColorButton(const QString & text, const QVector3D & color, QBoxLayout * layout) const;
    void updateColorButton(QPushButton * button, const QColor & color) const;
    void onColorButtonClicked(QPushButton * button, QVector3D& color, const QString& title);

    QColorDialog * colorPicker_;

    // position
	QSlider * fromX_;
	QSlider * fromY_;
	QSlider * size_;
    QSlider * maxIters_;
    // colors
    QSlider * brightness_;
	QSlider * contrast_;
    QPushButton * colorA_;
	QPushButton * colorB_;
    QPushButton * colorC_;
	QPushButton * colorD_;
    // layouts
    QGroupBox* positionsGroup = new QGroupBox("Position Parameters");
    QGroupBox* colorsGroup = new QGroupBox("Color Parameters");
    QVBoxLayout * colorsLayout;
    QVBoxLayout * positionsLayout;
    QLayout * mainLayout;
};