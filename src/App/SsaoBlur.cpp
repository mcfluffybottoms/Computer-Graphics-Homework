#include "App/SsaoBlur.h"

Blur::Blur(OpenGLContextPtr context, std::shared_ptr<SharedData> data) : context_(context), data_(data) {
    buffer_ = std::make_unique<IOBuffer>(context_, data->size.width(), data->size.height(), false);
    buffer_->bindNewColorTexture(data->size.width(), data->size.height(), GL_R32F);
    
    program_ = std::make_shared<QOpenGLShaderProgram>();
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/blur.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/blur.fs");
	program_->link();

    initUniformValues();
}

Blur::~Blur() {
    program_.reset();
}

std::shared_ptr<QOpenGLShaderProgram> Blur::getShader() {
    return program_;
}

void Blur::initUniformValues() {
    samplesUniform_ = program_->uniformLocation("samples");
    hasBlurUniform_ = program_->uniformLocation("hasBlur");
}

void Blur::setUniformValues() {
    program_->setUniformValue(samplesUniform_, data_->samples);
    program_->setUniformValue("colorMap", SSAO_UNIT_INDEX);
    program_->setUniformValue(hasBlurUniform_, data_->hasBlur);
}

void Blur::enable() {
	buffer_->write();
	program_->bind();
	setUniformValues();
}

void Blur::disable()  {
    program_->release();
}

void Blur::readBuffer() {
    buffer_->read(1, BLUR_UNIT);
}