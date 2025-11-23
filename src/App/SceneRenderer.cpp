#include "SceneRenderer.h"
#include "App/Camera.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <qboxlayout.h>

SceneRenderer::SceneRenderer(OpenGLContextPtr context)
	: context_(context)
{
	camera = new Camera();
	camera->position().setX(0);
	camera->position().setY(0);
	camera->position().setZ(1);
}

SceneRenderer::~SceneRenderer()
{
	if (camera)
		delete camera;
	if (entityModel)
		delete entityModel;
}

bool SceneRenderer::setShaders()
{
	program_ = std::make_shared<QOpenGLShaderProgram>();
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/diffuse.fs");
	program_->link();
	return true;
}

void SceneRenderer::initUniformValues()
{
	// Uniform values for shader
	entityModel->mvpUniform_ = program_->uniformLocation("mvp");
	entityModel->modelUniform_ = program_->uniformLocation("model");
	entityModel->normalMatrixUniform_ = program_->uniformLocation("normalMatrix");
	ambient_ = program_->uniformLocation("ambient");
	diffuse_ = program_->uniformLocation("diffuse");
	uniformLightPosition_ = program_->uniformLocation("lightPos");
	uniformLightColor_ = program_->uniformLocation("lightColor");
	uniformViewPos_ = program_->uniformLocation("viewPos");
}

void SceneRenderer::setUniformValues(const QMatrix4x4 & mvp)
{
	program_->setUniformValue(entityModel->mvpUniform_, mvp);
}


bool SceneRenderer::onInit(fgl::GLWidget * window)
{
	if (window)
		window->makeCurrent();
	auto * f = context_ ? context_->functions() : nullptr;

	setShaders();
	entityModel = new EntityModel(program_);
	entityModel->setScale(QVector3D(0.01f, 0.01f, 0.01f));
	entityModel->setPosition(QVector3D(0.0f, 0.0f, -1.0f));

	initUniformValues();

	entityModel->setImportedModel(":/Models/asset_1.glb");

	// Еnable depth test and face culling
	f->glEnable(GL_DEPTH_TEST);
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);
	f->glFrontFace(GL_CCW);

	// Clear all FBO buffers
	context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}

bool SceneRenderer::onRender(fgl::GLWidget * window, const QMatrix4x4 & mvp)
{
	// Clear buffers
	context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program_->bind();

	if (1.0f >= 0) // TODO: ambient input
		program_->setUniformValue(ambient_, 0.1f);
	if (1.0f >= 0) // TODO: diffuse input
		program_->setUniformValue(diffuse_, 1.0f);
	if (1.0f >= 0) // TODO: ambient input
		program_->setUniformValue(uniformLightColor_, lightColor_.x(), lightColor_.y(), lightColor_.z());
	if (1.0f >= 0) // TODO: diffuse input
		program_->setUniformValue(uniformLightPosition_, lightPosition_.x(), lightPosition_.y(), lightPosition_.z());
	if (1.0f >= 0) // TODO: diffuse input
		program_->setUniformValue(uniformViewPos_, camera->position().x(), camera->position().y(), camera->position().z());
		
	setUniformValues(mvp);

	entityModel->render(mvp, context_);

	program_->release();
	return true;
}

bool SceneRenderer::onResize(fgl::GLWidget * window)
{
	auto width = window->width();
	auto height = window->height();
	context_->functions()->glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
	return true;
}

void SceneRenderer::rotateObj(const QVector2D& delta) {

    float sensitivity = 0.05f;
    QVector3D rotation = entityModel->getRotation();
    rotation.setY(rotation.y() + delta.x() * sensitivity);
    rotation.setX(rotation.x() + delta.y() * sensitivity);

    entityModel->setRotation(rotation);
}