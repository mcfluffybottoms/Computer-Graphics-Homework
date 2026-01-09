#include "App/Camera.h"
#include <QVector3D>
#include <QSize>
#include <QMatrix4x4>

#define COLOR_UNIT GL_TEXTURE0
#define COLOR_UNIT_INDEX 0

#define POSITION_UNIT GL_TEXTURE1
#define POSITION_UNIT_INDEX 1

#define NORMAL_UNIT GL_TEXTURE2
#define NORMAL_UNIT_INDEX 2

#define ALBEDO_UNIT GL_TEXTURE3
#define ALBEDO_UNIT_INDEX 3

#define DEPTH_UNIT GL_TEXTURE4
#define DEPTH_UNIT_INDEX 4

#define SSAO_UNIT GL_TEXTURE5
#define SSAO_UNIT_INDEX 5

#define BLUR_UNIT GL_TEXTURE6
#define BLUR_UNIT_INDEX 6

#define NOISE_UNIT GL_TEXTURE7
#define NOISE_UNIT_INDEX 7

struct SharedData {
	// window size
	QSize size;
	
	// lights
	float ambient = 1.0f;
	float diffuse = 1.0f;
	float specular = 1.0f;

	float ambientP = 1.0f;
	float diffuseP = 1.0f;
	float specularP = 1.0f;

	QVector3D directionalLightColor = QVector3D(0.5f, 0.5f, 0.5f);
	QVector3D projectionLightColor = QVector3D(0.5f, 0.5f, 0.5f);

	QVector3D directionalLightPosition = QVector3D(0.5f, 0.5f, 0.5f);
	QVector3D projectionLightPosition = QVector3D(0.5f, 0.5f, 0.5f);

	QVector3D projectionLightDir = QVector3D(0.5f, 0.5f, 0.5f);

	float projCutOff{20.f};
	float projOuterCutOff{50.0f};

	bool hasDirectional = false;
	bool hasProjection = false;

	int samples = 10;
	int kernelSize = 64;
	float sampleRadius = 0.5f;
	float sigma = 1.0;
	float bias = 0.025;
	bool hasAO;
	bool hasBlur;
	bool debugAO;

	std::shared_ptr<Camera> currentCamera = nullptr;
};