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

	void setUniformLightValues(const QMatrix4x4 & mvp,
						  bool directional_,
						  bool projection_);

	void rotateObj(const QVector2D & rotation);


	Camera * camera = nullptr;

private:
	OpenGLContextPtr context_;

	// light
	void initLights();
	//DirectionalLight * dirLight;
	//ProjectionLight * projLight;
	GLboolean uniformDirectional_ = true;
	GLboolean uniformProjection_ = true;

	// render functions
	bool geometryPass(const QMatrix4x4 & mvp);
	bool SSAOPass();
	bool blurPass();
	bool lightPass();

	// shader setup
	std::shared_ptr<ShaderManager> shaderManager;

	// SSAO текстуры
    QOpenGLTexture* m_ssaoColorBuffer;
    QOpenGLTexture* m_ssaoBlurBuffer;

	// new buffers
	std::unique_ptr<QOpenGLFramebufferObject> depthBuffer_;
	std::unique_ptr<QOpenGLFramebufferObject> aoFBO_;
	std::unique_ptr<QOpenGLFramebufferObject> aoBlurFBO_;

	// model
	EntityModel * entityModel = nullptr;
};