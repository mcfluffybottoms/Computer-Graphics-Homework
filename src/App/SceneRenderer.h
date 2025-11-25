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
						  float morph, 
						  float radius);

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
	GLfloat uniformRadius_ = 1.0f;
	GLboolean uniformDirectional_ = true;
	GLboolean uniformProjection_ = true;

	GLint uniformViewPos_ = 1.0f;
};