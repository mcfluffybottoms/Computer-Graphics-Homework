#include "SceneRenderer.h"
#include "App/Camera.h"
#include "App/EntityModel.h"
#include "App/Light.h"
#include "App/SliderGroup.h"
#include <QBoxLayout>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <cmath>
#include <memory>
#include <qvector2d.h>


std::array<QVector3D, KERNEL_SIZE> genSSAOKernel()
{
	std::array<QVector3D, KERNEL_SIZE>  kernel;
	for (uint i = 0 ; i < KERNEL_SIZE ; i++ ) {
        float scale = (float)i / (float)(KERNEL_SIZE);        
        QVector3D v;
        v.setX(2.0f * (float)rand()/RAND_MAX - 1.0f);
        v.setY(2.0f * (float)rand()/RAND_MAX - 1.0f);
        v.setZ(2.0f * (float)rand()/RAND_MAX - 1.0f);
        v *= (0.1f + 0.9f * scale * scale);
        kernel[i] = v;
    }
	return kernel;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad(OpenGLContextPtr context_)
{
	auto * f = context_ ? context_->functions() : nullptr;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        f->glGenVertexArrays(1, &quadVAO);
        f->glGenBuffers(1, &quadVBO);
        f->glBindVertexArray(quadVAO);
        f->glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        f->glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        f->glEnableVertexAttribArray(0);
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        f->glEnableVertexAttribArray(1);
        f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    f->glBindVertexArray(quadVAO);
    f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    f->glBindVertexArray(0);
}


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
	dirLight = new DirectionalLight(shaderManager->m_lighting_program_, 0);
	projLight = new ProjectionLight(shaderManager->m_lighting_program_, 0);
}

bool SceneRenderer::onInit(fgl::GLWidget * window)
{
	if (window)
		window->makeCurrent();
	auto * f = context_ ? context_->functions() : nullptr;

	// Set up shaders
	shaderManager = std::make_shared<ShaderManager>();
	shaderManager->initUniformValues();

	// Load model
	entityModel = new EntityModel(shaderManager);
	entityModel->setScale(QVector3D(0.5f, 0.5f, 0.5f));
	entityModel->setPosition(QVector3D(0.0f, 0.0f, 0.0f));
	entityModel->setRotation({0.0f, 0.0f, 0.0f});
	entityModel->setImportedModel(":/Models/lois.glb");

	SSAOKernel = genSSAOKernel();

	// Set lights
	initLights();

	// Еnable depth test and face culling
	f->glEnable(GL_DEPTH_TEST);
	f->glDepthFunc(GL_LESS); 
	f->glDepthMask(GL_TRUE);
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);
	f->glFrontFace(GL_CCW);

	// Init depth buffers
	std::cout << "creating depthBuffer_..." << "\n";
	depthBuffer_ = std::make_unique<IOBuffer>(context_, window->width(), window->height(), GL_NONE);
	std::cout << "creating aoBuffer_..." << "\n";
	aoBuffer_ = std::make_unique<IOBuffer>(context_, window->width(), window->height(), GL_R32F);
	std::cout << "creating aoBlurBuffer_..." << "\n";
	aoBlurBuffer_ = std::make_unique<IOBuffer>(context_, window->width(), window->height(), GL_R32F);

	// Clear all FBO buffers
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}

SceneRenderer::Quad::Quad()
{
	std::vector<Vertex> vertices = {
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
		{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}};
	std::vector<uint32_t> indices = {
		0, 1, 2,
		1, 3, 2};
	mesh = ModelMesh{vertices, indices, -1};
}

void SceneRenderer::Quad::loadBuffers(std::shared_ptr<QOpenGLShaderProgram> program_)
{
	auto vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::Type::VertexBuffer);
	auto ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::Type::IndexBuffer);
	auto vao = std::make_unique<QOpenGLVertexArrayObject>();

	vao->create();
	vao->bind();

	vbo->create();
	vbo->bind();
	vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo->allocate(mesh.vertices.data(), static_cast<int>(mesh.vertices.size() * sizeof(Vertex)));

	ibo->create();
	ibo->bind();
	ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo->allocate(mesh.indices.data(), static_cast<int>(mesh.indices.size() * sizeof(uint32_t)));

	// 
	program_->bind();
    program_->enableAttributeArray(0);
    program_->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));
    program_->enableAttributeArray(1);
    program_->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));
    program_->enableAttributeArray(2);
    program_->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, texCoords), 2, sizeof(Vertex));
    program_->release();

	vao_ = std::move(vao);
	vbo_ = std::move(vbo);
	ibo_ = std::move(ibo);
}

void SceneRenderer::Quad::render(OpenGLContextPtr context)
{
	if (vao_)
	{
		vao_->bind();
		context->functions()->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
		vao_->release();
	}
}

bool SceneRenderer::geometryPass(const QMatrix4x4 & mvp)
{
	auto * f = context_ ? context_->functions() : nullptr;

	depthBuffer_->write();
	f->glEnable(GL_DEPTH_TEST);
	f->glDepthMask(GL_TRUE);
	f->glDepthFunc(GL_LESS); 
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);
	f->glFrontFace(GL_CCW);
	f->glClear(GL_DEPTH_BUFFER_BIT);

	shaderManager->m_geometry_program_->bind();
	shaderManager->setGeometryUniformValues(
		mvp * entityModel->getTransform(), 
		entityModel->getTransform()
	);

	entityModel->renderMesh(context_);

	shaderManager->m_geometry_program_->release();
	return true;
}

bool SceneRenderer::SSAOPass(const QSize& size, const QMatrix4x4 & projMatrix)
{
	auto * f = context_ ? context_->functions() : nullptr;

	depthBuffer_->read(GL_TEXTURE1);
	aoBuffer_->write();
	f->glEnable(GL_DEPTH_TEST);
	f->glDepthMask(GL_TRUE);
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);
	f->glFrontFace(GL_CCW);
	f->glClear(GL_COLOR_BUFFER_BIT);

	shaderManager->m_ao_program_->bind();
	shaderManager->setAOUniformValues(SSAOKernel, projMatrix, std::tan(camera->FOV()*3.14/360), ((float)size.width())/size.height());
	
	renderQuad(context_);

	shaderManager->m_ao_program_->release();
	return true;
}

bool SceneRenderer::blurPass(const QMatrix4x4 & mvp)
{
	return true;
}

void SceneRenderer::setLightUniforms(SlidersGroup * window) {
	if (dirLight)
	{
		dirLight->setUniformValues(
			window->getVector(1),
			window->directionalLightColor,
			window->ambient,
			window->diffuse,
			window->specular, shaderManager->m_lighting_program_);
	}

	if (projLight)
	{
		projLight->setUniformValues(
			window->getVector(2),
			window->projectionLightColor,
			camera->y(),
			window->ambientP,
			window->diffuseP,
			window->specularP,
			window->projCutOff,
			window->projOuterCutOff, shaderManager->m_lighting_program_);
	}

}

bool SceneRenderer::lightPass(const QSize& size, SlidersGroup * window, const QMatrix4x4 & mvp)
{
	aoBuffer_->read(GL_TEXTURE2);
	context_->functions()->glBindFramebuffer(GL_FRAMEBUFFER, 4); //?????
	context_->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderManager->m_lighting_program_->bind();
	
	setLightUniforms(window);

	shaderManager->setLightUniformValues(
		window->hasDirectional, 
		window->hasProjection, 
		mvp * entityModel->getTransform(), 
		entityModel->getTransform(),
		size
	);

	entityModel->render(shaderManager->m_lighting_program_, context_);
	//quad.loadBuffers(shaderManager->m_lighting_program_);
	//quad.render(context_);
	//renderQuad(context_);

	shaderManager->m_lighting_program_->release();
	return true;
}

bool SceneRenderer::onRender(const QSize& size, SlidersGroup * window, const QMatrix4x4 & mvp)
{
	geometryPass(mvp);
	SSAOPass(size, camera->getProjection());
	lightPass(size, window, mvp);
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