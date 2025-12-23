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


void SceneRenderer::setUniformLightValues(const QMatrix4x4 & mvp,
						  bool directional_,
						  bool projection_)
{
	// if (entityModel)
	// {
	// 	entityModel->mvpUniform_ = m_geometry_program_->uniformLocation("mvp");
	// 	entityModel->modelUniform_ = m_geometry_program_->uniformLocation("model");
	// 	entityModel->invertedTransposedMatrixUniform_ = m_geometry_program_->uniformLocation("itMatrix");
	// }
	//uniformViewPos_ = m_geometry_program_->uniformLocation("viewPos");
}

void SceneRenderer::initLights()
{
	//dirLight = new DirectionalLight(m_lighting_program_, 0);
	//projLight = new ProjectionLight(m_lighting_program_, 0);
}

bool SceneRenderer::onInit(fgl::GLWidget * window)
{
	if (window)
		window->makeCurrent();
	auto * f = context_ ? context_->functions() : nullptr;

	shaderManager = std::make_shared<ShaderManager>();

	entityModel = new EntityModel(shaderManager);
	entityModel->setScale(QVector3D(0.1f, 0.1f, 0.1f));
	entityModel->setPosition(QVector3D(0.0f, 0.0f, -1.0f));
	entityModel->setRotation({180.0f, 180.0f, 0.0f});

	shaderManager->initUniformValues();

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

bool SceneRenderer::geometryPass(const QMatrix4x4 & mvp) {
	//m_geometry_program_->bind();
	
	//context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// set uniform values in shader
	//m_geometry_program_->setUniformValue(entityModel->mvpUniform_, mvp);

}

bool SceneRenderer::onRender(SlidersGroup * window, const QMatrix4x4 & mvp)
{
	// Clear buffers
	//context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//program_->bind();
	geometryPass(mvp);

	// if (dirLight)
	// {
	// 	dirLight->setUniformValues(
	// 		window->getVector(1),
	// 		window->directionalLightColor,
	// 		window->ambient,
	// 		window->diffuse,
	// 		window->specular, program_);
	// }

	// if (projLight)
	// {
	// 	projLight->setUniformValues(
	// 		window->getVector(2),
	// 		window->projectionLightColor,
	// 		camera->y(),
	// 		window->ambientP,
	// 		window->diffuseP,
	// 		window->specularP,
	// 		window->projCutOff,
	// 		window->projOuterCutOff, program_);
	// }

	entityModel->render(mvp, context_);

	//program_->release();
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