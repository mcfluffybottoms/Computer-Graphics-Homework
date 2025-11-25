#include "SceneRenderer.h"
#include "App/Camera.h"
#include "App/Light.h"
#include "App/SliderGroup.h"
#include <QBoxLayout>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

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
	if (entityModel)
	{
		entityModel->mvpUniform_ = program_->uniformLocation("mvp");
		entityModel->modelUniform_ = program_->uniformLocation("model");
		entityModel->invertedTransposedMatrixUniform_ = program_->uniformLocation("itMatrix");
	}

	uniformDirectional_ = program_->uniformLocation("hasDirectional");
	uniformProjection_ = program_->uniformLocation("hasProjection");
	uniformViewPos_ = program_->uniformLocation("viewPos");
	uniformMorph_ = program_->uniformLocation("morphIntensity");
}

void SceneRenderer::initLights()
{
	dirLight = new DirectionalLight(program_, 0);
	projLight = new ProjectionLight(program_, 0);
}

void SceneRenderer::setUniformValues(const QMatrix4x4 & mvp,
									 bool directional_,
									 bool projection_,
									 float morph)
{
	if (entityModel)
		program_->setUniformValue(entityModel->mvpUniform_, mvp);

	program_->setUniformValue(uniformDirectional_, directional_);
	program_->setUniformValue(uniformProjection_, projection_);
	program_->setUniformValue(uniformMorph_, morph);
	program_->setUniformValue(uniformViewPos_, camera->position());
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
	initLights();

	entityModel->setImportedModel(":/Models/sbob.glb");

	// Еnable depth test and face culling
	f->glEnable(GL_DEPTH_TEST);
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);
	f->glFrontFace(GL_CCW);

	// Clear all FBO buffers
	context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}

bool SceneRenderer::onRender(SlidersGroup * window, const QMatrix4x4 & mvp)
{
	// Clear buffers
	context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	program_->bind();


	setUniformValues(mvp,
					 window->hasDirectional,
					 window->hasProjection,
					 window->morph);

	if (dirLight)
	{
		dirLight->setUniformValues(
			window->getVector(1),
			window->directionalLightColor,
			window->ambient,
			window->diffuse,
			window->specular, program_);
	}

	if (projLight)
	{
		projLight->setUniformValues(
			window->getVector(2),
			window->projectionLightColor,
			camera->y(),
			window->ambient,
			window->diffuse,
			window->specular,
			window->projCutOff,
			window->projOuterCutOff, program_);
	}

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

void SceneRenderer::rotateObj(const QVector2D & delta)
{

	float sensitivity = 0.05f;
	QVector3D rotation = entityModel->getRotation();
	rotation.setY(rotation.y() + delta.x() * sensitivity);
	rotation.setX(rotation.x() + delta.y() * sensitivity);

	entityModel->setRotation(rotation);
}