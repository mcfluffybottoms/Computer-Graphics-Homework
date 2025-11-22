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

private:
	OpenGLContextPtr context_;

	void initUniformValues();

	// std::vector<QOpenGLBuffer> vbos_;
	// std::vector<QOpenGLBuffer> ibos_;
	// std::vector<QOpenGLVertexArrayObject> vaos_;
	std::shared_ptr<QOpenGLShaderProgram> program_;

	EntityModel * entityModel = nullptr;
};