#include "SceneRenderer.h"
#include "App/Camera.h"
#include "App/EntityModel.h"
#include "App/Light.h"
#include "App/SliderGroup.h"
#include <QBoxLayout>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <memory>
#include <qvector2d.h>
#include <iostream>

GLint default_fbo_ = 0;

SceneRenderer::Quad::Quad() {}

void SceneRenderer::Quad::loadBuffers(OpenGLContextPtr context) {
	auto * f = context->functions();
	f->glGenVertexArrays(1, &quadVAO);
	f->glGenBuffers(1, &quadVBO);
	f->glBindVertexArray(quadVAO);
	f->glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	f->glEnableVertexAttribArray(0);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void SceneRenderer::Quad::render(OpenGLContextPtr context_) {
	auto * f = context_ ? context_->functions() : nullptr;
    if (quadVAO == 0) {
        loadBuffers(context_);
    }
    f->glBindVertexArray(quadVAO);
    f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    f->glBindVertexArray(0);
}

SceneRenderer::LightBlock::LightBlock() {
	program_ = std::make_shared<QOpenGLShaderProgram>();
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/phonglight.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/phonglight.fs");
	program_->link();

	dirLight = new DirectionalLight(program_, 0);
	projLight = new ProjectionLight(program_, 0);

	hasDirectionalUniform_= program_->uniformLocation("hasDirectional");
	hasProjectionUniform_= program_->uniformLocation("hasProjection");
	debugAOUniform_ = program_->uniformLocation("debugAO");
	screenSizeUniform_= program_->uniformLocation("screenSize");
}

SceneRenderer::LightBlock::~LightBlock() {
	program_.reset();
	if(dirLight) {
		delete dirLight;
	}
	if(projLight) {
		delete projLight;
	}
}

void SceneRenderer::LightBlock::setUniforms(std::shared_ptr<SharedData> data) {
	if(!data->currentCamera) {
		return;
	}
	if (dirLight)
	{
		dirLight->setUniformValues(
			data->directionalLightPosition,
			data->directionalLightColor,
			data->ambient,
			data->diffuse,
			data->specular, program_);
	}
	if (projLight)
	{
		projLight->setUniformValues(
			data->projectionLightPosition,
			data->projectionLightColor,
			data->currentCamera->y(),
			data->ambientP,
			data->diffuseP,
			data->specularP,
			data->projCutOff,
			data->projOuterCutOff, program_);
	}
    program_->setUniformValue(hasDirectionalUniform_, data->hasDirectional);
    program_->setUniformValue(hasProjectionUniform_, data->hasProjection);
	program_->setUniformValue(debugAOUniform_, data->debugAO);
	program_->setUniformValue(screenSizeUniform_, data->size);

	program_->setUniformValue("aoMap", BLUR_UNIT_INDEX);
	program_->setUniformValue("colorMap", COLOR_UNIT_INDEX);
	program_->setUniformValue("positionMap", POSITION_UNIT_INDEX);
	program_->setUniformValue("normalMap", NORMAL_UNIT_INDEX);
	program_->setUniformValue("albedoMap", ALBEDO_UNIT_INDEX);

}

void SceneRenderer::LightBlock::enable() {
	program_->bind();
}

void SceneRenderer::LightBlock::disable() {
	program_->release();
}

std::shared_ptr<QOpenGLShaderProgram> SceneRenderer::LightBlock::getShader() {
	return program_;
}

SceneRenderer::SceneRenderer(std::shared_ptr<SharedData> data, OpenGLContextPtr context)
	: camera_(std::make_shared<Camera>()), context_(context), data_(data)
{
	camera_->position().setX(0);
	camera_->position().setY(0);
	camera_->position().setZ(1);

	data_->currentCamera = camera_;
}

SceneRenderer::~SceneRenderer()
{
	if (entityModel)
		delete entityModel;
}

bool SceneRenderer::onInit(fgl::GLWidget * window)
{
	if (window)
		window->makeCurrent();
	auto * f = context_ ? context_->functions() : nullptr;

	// Set up
	lightBlock = std::make_unique<LightBlock>();
	geometryPassC = std::make_unique<GeometryPass>(context_, data_);
	ambientOcclusionC = std::make_unique<AmbientOcclusion>(context_, data_);
	blurC = std::make_unique<Blur>(context_, data_);

	// Load model
	entityModel = new EntityModel();
	entityModel->setScale(QVector3D(0.5f, 0.5f, 0.5f));
	entityModel->setPosition(QVector3D(0.0f, 0.0f, 0.0f));
	entityModel->setRotation({-60.0f, -45.0f, 45.0f});
	entityModel->setImportedModel(":/Models/steam_engine.glb");
	entityModel->loadBuffers(lightBlock->getShader());
	//entityModel->render(context_, 0);

	// Еnable depth test and face culling
	f->glEnable(GL_DEPTH_TEST);
	f->glDepthFunc(GL_LESS); 
	f->glDepthMask(GL_TRUE);
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);
	f->glFrontFace(GL_CCW);

	// Clear all FBO buffers
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

bool SceneRenderer::geometryPass()
{
	geometryPassC->enable(entityModel->getTransform());
	
	entityModel->render(context_, COLOR_UNIT_INDEX);

	geometryPassC->disable();

	geometryPassC->readDepthBuffer();
	geometryPassC->readPositionBuffer();
	geometryPassC->readNormalBuffer();
	geometryPassC->readAlbedoBuffer();

	return true;
}

bool SceneRenderer::SSAOPass()
{
	ambientOcclusionC->enable();
	
	quad.render(context_);

	ambientOcclusionC->disable();
	return true;
}

bool SceneRenderer::blurPass()
{
	ambientOcclusionC->readBuffer();
	blurC->enable();
	
	quad.render(context_);

	ambientOcclusionC->disable();
	return true;
}

bool SceneRenderer::lightPass(GLint default_framebuffer)
{
	blurC->readBuffer();

	context_->functions()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, default_framebuffer); //?????
	context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	lightBlock->enable();
	lightBlock->setUniforms(data_);
	
	quad.render(context_);

	lightBlock->disable();
	return true;
}

bool SceneRenderer::onRender(GLint default_framebuffer)
{
	 // TODO
	geometryPass();
	SSAOPass();
	blurPass();
	lightPass(default_framebuffer);
	return true;
}

bool SceneRenderer::onResize(fgl::GLWidget * window)
{
	auto width = window->width();
	auto height = window->height();
	context_->functions()->glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
	return true;
}

void SceneRenderer::rotateObj(const QVector2D & delta)
{

	float sensitivity = 0.5f;
	QVector3D rotation = entityModel->getRotation();
	rotation.setY(rotation.y() + delta.x() * sensitivity);
	rotation.setX(rotation.x() + delta.y() * sensitivity);

	entityModel->setRotation(rotation);
}