#pragma once

#include "App/IOBuffer.h"
#include "App/Light.h"
#include "App/Resources.h"
#include "App/SliderGroup.h"
#include "EntityModel.h"
#include <Base/GLWidget.hpp>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <memory>

#define KERNEL_SIZE 64

struct SceneRenderer {
	SceneRenderer(OpenGLContextPtr context = nullptr);

	~SceneRenderer();

	bool onInit(fgl::GLWidget * window);

	bool onRender(const QSize& size, SlidersGroup * window, const QMatrix4x4 & mvp);

	bool onResize(fgl::GLWidget * window);

	void setUniformLightValues(const QMatrix4x4 & mvp,
							   bool directional_,
							   bool projection_);

	void rotateObj(const QVector2D & rotation);


	Camera * camera = nullptr;

private:
	OpenGLContextPtr context_;

	// Render Quad
	struct Quad {
		Quad();
		void loadBuffers(std::shared_ptr<QOpenGLShaderProgram> program_);
		void render(OpenGLContextPtr context);
	  private:
		ModelMesh mesh;
		std::unique_ptr<QOpenGLBuffer> vbo_;
		std::unique_ptr<QOpenGLBuffer> ibo_;
		std::unique_ptr<QOpenGLVertexArrayObject> vao_;
	};
	Quad quad;
	std::array<QVector3D, KERNEL_SIZE> SSAOKernel;

	// light
	void initLights();
	DirectionalLight* dirLight;
	ProjectionLight* projLight;
	GLboolean uniformDirectional_ = true;
	GLboolean uniformProjection_ = true;
	void setLightUniforms(SlidersGroup * window);

	// render functions
	bool geometryPass(const QMatrix4x4 & mvp);
	bool SSAOPass(const QSize& size, const QMatrix4x4 & projMatrix);
	bool blurPass(const QMatrix4x4 & mvp);
	bool lightPass(const QSize& size, SlidersGroup* window, const QMatrix4x4 & mvp);

	// shader setup
	std::shared_ptr<ShaderManager> shaderManager;

	// SSAO текстуры
	QOpenGLTexture * m_ssaoColorBuffer;
	QOpenGLTexture * m_ssaoBlurBuffer;

	// new buffers
	std::unique_ptr<IOBuffer> depthBuffer_;
	std::unique_ptr<IOBuffer> aoBuffer_;
	std::unique_ptr<IOBuffer> aoBlurBuffer_;

	// model
	EntityModel * entityModel = nullptr;
};