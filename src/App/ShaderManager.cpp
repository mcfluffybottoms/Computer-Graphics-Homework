#include "ShaderManager.h"
#include "Resources.h"
#include <QMatrix4x4>

ShaderManager::ShaderManager() {
    // geometry
	m_geometry_program_ = std::make_shared<QOpenGLShaderProgram>();
	m_geometry_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/geometry.vs");
	m_geometry_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/geometry.fs");
	m_geometry_program_->link();
	// ambient occlusion
	m_ao_program_ = std::make_shared<QOpenGLShaderProgram>();
	m_ao_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/phonglight.vs");
	m_ao_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/phonglight.fs");
	m_ao_program_->link();
	// blur
	m_blur_program_ = std::make_shared<QOpenGLShaderProgram>();
	m_blur_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/phonglight.vs");
	m_blur_program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/phonglight.fs");
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
    
}
void ShaderManager::initBlurUniformValues() {
    
}
void ShaderManager::initLightUniformValues() {
    
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

void ShaderManager::setAOUniformValues() {
    
}
void ShaderManager::setBlurUniformValues() {
    
}
void ShaderManager::setLightUniformValues() {
    
}
