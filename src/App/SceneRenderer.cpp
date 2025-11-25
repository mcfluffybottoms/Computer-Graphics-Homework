#include "SceneRenderer.h"
#include "App/Camera.h"
#include "App/SliderGroup.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <iostream>
#include <qboxlayout.h>
#include <qmath.h>
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

	uniformAmbient_ = program_->uniformLocation("ambient");
	uniformDiffuse_ = program_->uniformLocation("diffuse");
	uniformSpecular_ = program_->uniformLocation("specular");

	uniformLightPosition_ = program_->uniformLocation("lightPos");
	uniformLightColor_ = program_->uniformLocation("lightColor");

	uniformProjLightPosition_ = program_->uniformLocation("projLightPos");
	uniformProjLightColor_ = program_->uniformLocation("projLightColor");

	uniformProjCutOff_ = program_->uniformLocation("projCutOff");
	uniformProjOuterCutOff_ = program_->uniformLocation("projOuterCutOff");
	uniformProjLightDir_ = program_->uniformLocation("projLightDir");

	uniformDirectional_ = program_->uniformLocation("hasDirectional");
	uniformProjection_ = program_->uniformLocation("hasProjection");

	uniformViewPos_ = program_->uniformLocation("viewPos");

	uniformMorph_ = program_->uniformLocation("morphIntensity");
}

void SceneRenderer::setUniformValues(const QMatrix4x4 & mvp, const QVector3D& lightPosition_,
									 const QVector3D& lightColor_,
									 const QVector3D& projLightPosition_,
									 const QVector3D& projLightColor_,
									 const QVector3D& projLightDirection_,
									 float projCutOff_,
									 float projOuterCutOff_,

									 float ambient_,
									 float diffuse_,
									 float specular_,

									 bool directional_,
									 bool projection_,
									 float morph)
{
	program_->setUniformValue(entityModel->mvpUniform_, mvp);
	program_->setUniformValue(uniformAmbient_, ambient_);
	program_->setUniformValue(uniformDiffuse_, diffuse_);
	program_->setUniformValue(uniformSpecular_, specular_);
	program_->setUniformValue(uniformLightColor_, lightColor_.x(), lightColor_.y(), lightColor_.z());
	program_->setUniformValue(uniformLightPosition_, lightPosition_.x(), lightPosition_.y(), lightPosition_.z());
	program_->setUniformValue(uniformProjLightPosition_, projLightPosition_.x(), projLightPosition_.y(), projLightPosition_.z());
	program_->setUniformValue(uniformProjLightColor_, projLightColor_);
	program_->setUniformValue(uniformProjLightDir_, projLightDirection_.normalized());
	program_->setUniformValue(uniformProjCutOff_, std::cos(qDegreesToRadians(projCutOff_)));
	program_->setUniformValue(uniformProjOuterCutOff_, std::cos(qDegreesToRadians(projOuterCutOff_)));
	program_->setUniformValue(uniformDirectional_, directional_);
	program_->setUniformValue(uniformProjection_, projection_);
	program_->setUniformValue(uniformViewPos_, camera->position());

	program_->setUniformValue(uniformMorph_, morph);
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

	setUniformValues(mvp, window->getVector(1),
						window->directionalLightColor,
						window->getVector(2),
						window->projectionLightColor,
						camera->y(),
						window->projCutOff,
						window->projOuterCutOff,
						window->ambient,
						window->diffuse,
						window->specular,
						window->hasDirectional,
						window->hasProjection,
						 window->morph
	);

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