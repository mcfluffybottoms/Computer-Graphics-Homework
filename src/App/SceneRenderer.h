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
						  bool projection_);

	void rotateObj(const QVector2D & rotation);


	Camera * camera = nullptr;

private:
	OpenGLContextPtr context_;

	void initLights();
	void initUniformValues();

	// render functions
	bool geometryPass(const QMatrix4x4 & mvp);
	bool SSAOPass();
	bool blurPass();
	bool lightPass();

	// shader setup
	std::shared_ptr<QOpenGLShaderProgram> m_geometry_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_ao_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_blur_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_lighting_program_;

	// SSAO текстуры
    QOpenGLTexture* m_ssaoColorBuffer;
    QOpenGLTexture* m_ssaoBlurBuffer;

	// new buffers
	std::unique_ptr<QOpenGLFramebufferObject> depthBuffer_;
	std::unique_ptr<QOpenGLFramebufferObject> aoFBO_;
	std::unique_ptr<QOpenGLFramebufferObject> aoBlurFBO_;

	// light
	DirectionalLight * dirLight;
	ProjectionLight * projLight;
	GLboolean uniformDirectional_ = true;
	GLboolean uniformProjection_ = true;

	// model
	EntityModel * entityModel = nullptr;
	GLint uniformViewPos_ = 1.0f;
};