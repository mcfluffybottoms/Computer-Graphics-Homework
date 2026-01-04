#include "ShaderManager.h"
#include "Resources.h"
#include <QMatrix4x4>
#include <QVector2D>

ShaderManager::ShaderManager() {
    // geometry
	m_geometry_program_ = std::make_shared<QOpenGLShaderProgram>();
	m_geometry_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/geometry.vs");
	m_geometry_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/geometry.fs");
	m_geometry_program_->link();
	// ambient occlusion
	m_ao_program_ = std::make_shared<QOpenGLShaderProgram>();
	m_ao_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/ao.vs");
	m_ao_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/ao.fs");
	m_ao_program_->link();
	// blur
	m_blur_program_ = std::make_shared<QOpenGLShaderProgram>();
	m_blur_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/blur.vs");
	m_blur_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/blur.fs");
	m_blur_program_->link();
	// lighting
	m_lighting_program_ = std::make_shared<QOpenGLShaderProgram>();
	m_lighting_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/phonglight.vs");
	m_lighting_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/phonglight.fs");
	m_lighting_program_->link();
}

ShaderManager::~ShaderManager() {
    m_geometry_program_.reset();
	m_ao_program_.reset();
	m_blur_program_.reset();
	m_lighting_program_.reset();
}

void ShaderManager::initUniformValues() {
    initGeometryUniformValues();
    initAOUniformValues();
    initBlurUniformValues();
    initLightUniformValues();
}
void ShaderManager::initGeometryUniformValues() {
    mvpUniform_ = m_geometry_program_->uniformLocation("mvp");
	modelUniform_ = m_geometry_program_->uniformLocation("model");
	invertedTransposedMatrixUniform_ = m_geometry_program_->uniformLocation("itMatrix");
}
void ShaderManager::initAOUniformValues() {
	// necessary data
	kernelUniform_ = m_ao_program_->uniformLocation("gKernel");
    projMatrixUniform_ = m_ao_program_->uniformLocation("gProj");
	aspectUniform_ = m_ao_program_->uniformLocation("gAspectRatio");
    tanHalfFOVUniform_ = m_ao_program_->uniformLocation("gTanHalfFOV");

	m_ao_program_->setUniformValue("gDepthMap", 1);
}
void ShaderManager::initBlurUniformValues() {
    
}
void ShaderManager::initLightUniformValues() {
	hasDirectionalUniform_ = m_lighting_program_->uniformLocation("hasDirectional");
    hasProjectionUniform_ = m_lighting_program_->uniformLocation("hasProjection");
	mvpLightUniform_ = m_lighting_program_->uniformLocation("mvp");
	modelLightUniform_ = m_lighting_program_->uniformLocation("model");
	invertedTransposedMatrixLightUniform_ = m_lighting_program_->uniformLocation("itMatrix");
}

void ShaderManager::setGeometryUniformValues(const QMatrix4x4& mvp, const QMatrix4x4& transform) {
    m_geometry_program_->setUniformValue(mvpUniform_, mvp);
    m_geometry_program_->setUniformValue(modelUniform_, transform);
    m_geometry_program_->setUniformValue(invertedTransposedMatrixUniform_, transform.inverted().transposed());
}

void ShaderManager::bindGeometryAttributes() {
    m_geometry_program_->bind();

    m_geometry_program_->enableAttributeArray(0);
    m_geometry_program_->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));

    m_geometry_program_->enableAttributeArray(1);
    m_geometry_program_->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));

    m_geometry_program_->enableAttributeArray(2);
    m_geometry_program_->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, texCoords), 2, sizeof(Vertex));

    // Release all
    m_geometry_program_->release();
}

void ShaderManager::setAOUniformValues(const std::array<QVector3D, 64> & kernel, const QMatrix4x4& projMatrix, float tanHalfFOV, float aspectRatio) {
	m_ao_program_->setUniformValueArray(kernelUniform_, kernel.data(), 64); // cahnge magic number
    m_ao_program_->setUniformValue(projMatrixUniform_, projMatrix);
	m_ao_program_->setUniformValue(aspectUniform_, aspectRatio); // cahnge magic number
    m_ao_program_->setUniformValue(tanHalfFOVUniform_, tanHalfFOV);

	m_ao_program_->setUniformValue("gDepthMap", 1);
}

void ShaderManager::setBlurUniformValues() {
    
}

void ShaderManager::setLightUniformValues(bool dir, bool proj, const QMatrix4x4& mvp, const QMatrix4x4& transform, const QSize& size) {
    m_lighting_program_->setUniformValue(hasDirectionalUniform_, dir);
    m_lighting_program_->setUniformValue(hasProjectionUniform_, proj);

	m_lighting_program_->setUniformValue(mvpLightUniform_, mvp);
    m_lighting_program_->setUniformValue(modelLightUniform_, transform);
    m_lighting_program_->setUniformValue(invertedTransposedMatrixLightUniform_, transform.inverted().transposed());

	m_lighting_program_->setUniformValue("screenSize", size);

	m_lighting_program_->setUniformValue("ao_map", 2);
}
