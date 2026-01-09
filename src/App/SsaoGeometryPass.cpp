#include "App/SsaoGeometryPass.h"

GeometryPass::GeometryPass(OpenGLContextPtr context, std::shared_ptr<SharedData> data) : context_(context), data_(data) {
    buffer_ = std::make_unique<IOBuffer>(context_, data_->size.width(), data_->size.height(), true);
    buffer_->bindNewColorTexture(data_->size.width(), data_->size.height(), GL_RGB32F);
	buffer_->bindNewColorTexture(data_->size.width(), data_->size.height(), GL_RGB32F);
	buffer_->bindNewColorTexture(data_->size.width(), data_->size.height(), GL_RGBA16F);

    program_ = std::make_shared<QOpenGLShaderProgram>();
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/geometry.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/geometry.fs");
	program_->link();

    initUniformValues();
}

GeometryPass::~GeometryPass() {
    program_.reset();
}

std::shared_ptr<QOpenGLShaderProgram> GeometryPass::getShader() {
    return program_;
}

void GeometryPass::initUniformValues() {
	projectionUniform_ = program_->uniformLocation("projection");
    viewUniform_ = program_->uniformLocation("view");
	modelUniform_ = program_->uniformLocation("model");
	invertedTransposedMatrixUniform_ = program_->uniformLocation("itMatrix");
}

void GeometryPass::setUniformValues(const QMatrix4x4& transform) {
    program_->setUniformValue(viewUniform_, data_->currentCamera->getView());
	program_->setUniformValue(projectionUniform_, data_->currentCamera->getProjection());
    program_->setUniformValue(modelUniform_, transform);
    program_->setUniformValue(invertedTransposedMatrixUniform_, transform.inverted().transposed());
}

void GeometryPass::enable(const QMatrix4x4 & transform) {
    auto * f = context_ ? context_->functions() : nullptr;

	buffer_->write();
	f->glEnable(GL_DEPTH_TEST);
	f->glDepthMask(GL_TRUE);
	f->glDepthFunc(GL_LESS); 
	f->glEnable(GL_CULL_FACE);
	f->glCullFace(GL_BACK);
	f->glFrontFace(GL_CCW);
	f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program_->bind();

	setUniformValues(
		transform
	);
}

void GeometryPass::disable() {
    program_->release();
}

void GeometryPass::readDepthBuffer() {
	buffer_->read(0, DEPTH_UNIT);
}
void GeometryPass::readPositionBuffer() {
	buffer_->read(1, POSITION_UNIT);
}
void GeometryPass::readNormalBuffer() {
	buffer_->read(2, NORMAL_UNIT);
}
void GeometryPass::readAlbedoBuffer() {
	buffer_->read(3, ALBEDO_UNIT);
}