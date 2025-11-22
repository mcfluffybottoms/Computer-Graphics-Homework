#include "SceneRenderer.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <qboxlayout.h>

SceneRenderer::SceneRenderer(OpenGLContextPtr context)
	: context_(context)
{
}

SceneRenderer::~SceneRenderer()
{
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
	entityModel->setScale(QVector3D(0.1f, 0.1f, 0.1f));
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