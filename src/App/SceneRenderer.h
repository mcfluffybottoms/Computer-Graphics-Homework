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

#include "App/SsaoGeometryPass.h"
#include "App/SsaoAmbientOcclusion.h"
#include "App/SsaoBlur.h"

struct SceneRenderer {
	SceneRenderer(std::shared_ptr<SharedData> data, OpenGLContextPtr context = nullptr);

	~SceneRenderer();

	bool onInit(fgl::GLWidget * window);

	bool onRender(GLint default_framebuffer);

	bool onResize(fgl::GLWidget * window);

	void rotateObj(const QVector2D & rotation);


	Camera * camera_ = nullptr;

private:
	OpenGLContextPtr context_;

	std::unique_ptr<GeometryPass> geometryPassC;
	std::unique_ptr<AmbientOcclusion> ambientOcclusionC;
	std::unique_ptr<Blur> blurC;

	// Render Quad
	struct Quad {
		Quad();
		void loadBuffers(OpenGLContextPtr context);
		void render(OpenGLContextPtr context);
	  private:
		float vertices[20] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
		unsigned int quadVAO = 0;
		unsigned int quadVBO;
	};
	Quad quad;

	// Light data for scene
	struct LightBlock {
		LightBlock();
		~LightBlock();
		void setUniforms(std::shared_ptr<SharedData> data);
		void enable();
		void disable();
		std::shared_ptr<QOpenGLShaderProgram> getShader();
	  private:
		DirectionalLight* dirLight;
		ProjectionLight* projLight;
		GLboolean hasDirectionalUniform_ = -1;
		GLboolean hasProjectionUniform_ = -1;
		GLint debugAOUniform_ = -1;
		GLint screenSizeUniform_ = -1;
		std::shared_ptr<QOpenGLShaderProgram> program_;
	};
	std::unique_ptr<LightBlock> lightBlock;

	// render functions
	bool geometryPass();
	bool SSAOPass();
	bool blurPass();
	bool lightPass(GLint default_framebuffer);

	// model
	EntityModel * entityModel = nullptr;

	// shared data
	std::shared_ptr<SharedData> data_;
};