#pragma once

#include "App/SliderGroup.h"
#include "EntityModel.h"
#include <Base/GLWidget.hpp>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <memory>

struct SceneRenderer {
	SceneRenderer(OpenGLContextPtr context = nullptr);

	~SceneRenderer();

	bool onInit(fgl::GLWidget * window);

	bool onRender(SlidersGroup * window, const QMatrix4x4 & mvp);

	bool onResize(fgl::GLWidget * window);

	bool setShaders();
	void setUniformValues(const QMatrix4x4 & mvp, const QVector3D & lightPosition_,
						  const QVector3D & lightColor_,
						  const QVector3D & projLightPosition_,
						  const QVector3D & projLightColor_,
						  const QVector3D & projLightDirection_,
						  float projCutOff_,
						  float projOuterCutOff_,

						  float ambient_,
						  float diffuse_,
						  float specular_,

						  bool directional_,
						  bool projection_,
						  float morph);

	void rotateObj(const QVector2D & rotation);


	Camera * camera = nullptr;

private:
	OpenGLContextPtr context_;

	void initUniformValues();

	std::shared_ptr<QOpenGLShaderProgram> program_;

	EntityModel * entityModel = nullptr;

	QVector3D lightPosition_{10.f, -15.f, 20.f};
	QVector3D lightColor_{1.0f, 0.0f, 0.5f};

	QVector3D projLightPosition_{10.f, -15.f, 20.f};
	QVector3D projLightColor_{1.0f, 1.0f, 0.5f};
	float projCutOff_{20.f};
	float projOuterCutOff_{50.0f};

	float ambient_ = 0.2f;
	float diffuse_ = 1.0f;
	float specular_ = 0.4f;

	bool directional_ = true;
	bool projection_ = true;

	GLfloat uniformMorph_ = 0.0f;

	GLint uniformLightPosition_ = 1.0f;
	GLint uniformLightColor_ = 1.0f;

	GLint uniformProjLightPosition_ = 1.0f;
	GLint uniformProjLightColor_ = 1.0f;
	GLint uniformProjCutOff_ = 1.0f;
	GLint uniformProjOuterCutOff_ = 1.0f;
	GLint uniformProjLightDir_ = 1.0f;

	GLint uniformViewPos_ = 1.0f;
	GLfloat uniformAmbient_ = 1.0f;
	GLfloat uniformDiffuse_ = 1.0f;
	GLfloat uniformSpecular_ = 1.0f;

	GLboolean uniformDirectional_ = true;
	GLboolean uniformProjection_ = true;
};