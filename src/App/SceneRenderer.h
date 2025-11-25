#pragma once

#include "App/Light.h"
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
	void setUniformValues(const QMatrix4x4 & mvp,
						  bool directional_,
						  bool projection_,
						  float morph);

	void rotateObj(const QVector2D & rotation);


	Camera * camera = nullptr;

private:
	OpenGLContextPtr context_;

	void initLights();
	void initUniformValues();

	DirectionalLight * dirLight;
	ProjectionLight * projLight;

	std::shared_ptr<QOpenGLShaderProgram> program_;

	EntityModel * entityModel = nullptr;

	GLfloat uniformMorph_ = 0.0f;
	GLboolean uniformDirectional_ = true;
	GLboolean uniformProjection_ = true;

	GLint uniformViewPos_ = 1.0f;
	// GLint uniformLightPosition_ = 1.0f;
	// GLint uniformLightColor_ = 1.0f;

	// GLint uniformProjLightPosition_ = 1.0f;
	// GLint uniformProjLightColor_ = 1.0f;
	// GLint uniformProjCutOff_ = 1.0f;
	// GLint uniformProjOuterCutOff_ = 1.0f;
	// GLint uniformProjLightDir_ = 1.0f;

	// GLint uniformViewPos_ = 1.0f;
	// GLfloat uniformAmbient_ = 1.0f;
	// GLfloat uniformDiffuse_ = 1.0f;
	// GLfloat uniformSpecular_ = 1.0f;
};