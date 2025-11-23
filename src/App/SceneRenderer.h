#pragma once

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

	bool onRender(fgl::GLWidget * window, const QMatrix4x4 & mvp);

	bool onResize(fgl::GLWidget * window);

	bool setShaders();
    void setUniformValues(const QMatrix4x4 & mvp);

	void rotateObj(const QVector2D& rotation);

	
	Camera* camera = nullptr;
private:
	OpenGLContextPtr context_;

	void initUniformValues();

	std::shared_ptr<QOpenGLShaderProgram> program_;

	EntityModel * entityModel = nullptr;

	QVector3D lightPosition_{1.2f, -1.5f, 2.0f};
	QVector3D lightColor_{1.0f, 0.0f, 0.5f};

	GLint uniformLightPosition_ = 1.0f;
	GLint uniformLightColor_ = 1.0f;
	GLint uniformViewPos_ = 1.0f;
	GLfloat ambient_ = 1.0f;
	GLfloat diffuse_ = 1.0f;

};