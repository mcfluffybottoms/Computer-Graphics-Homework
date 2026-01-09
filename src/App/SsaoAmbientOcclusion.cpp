#include "App/SsaoAmbientOcclusion.h"
#include <qopengltexture.h>
#include <cmath>

AmbientOcclusion::AmbientOcclusion(OpenGLContextPtr context, std::shared_ptr<SharedData> data) : context_(context), data_(data) {
    buffer_ = std::make_unique<IOBuffer>(context_, data->size.width(), data->size.height(), false);
    buffer_->bindNewColorTexture(data->size.width(), data->size.height(), GL_R32F);

    program_ = std::make_shared<QOpenGLShaderProgram>();
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/ao.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/ao.fs");
	program_->link();

    initUniformValues();
    SSAOKernel = genSSAOKernel();
    noise = genNoise();
    noise->bind(NOISE_UNIT_INDEX);
}

AmbientOcclusion::~AmbientOcclusion() {
    program_.reset();
}

std::unique_ptr<QOpenGLTexture> AmbientOcclusion::genNoise() {
    std::vector<float> noise(4 * 4 * 3);
    for (unsigned int i = 0; i < 16; i++)
    {
        noise[i] = 2.0f * (float)rand() / RAND_MAX - 1.0f;
        noise[i + 1] = 2.0f * (float)rand() / RAND_MAX - 1.0f;
        noise[i + 2] = 0.0f;     
    }
    auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    texture->setSize(4, 4);
    texture->setFormat(QOpenGLTexture::RGB32F);
    texture->allocateStorage();
    texture->setData(0, 0, 
        QOpenGLTexture::RGB, 
        QOpenGLTexture::Float32,
        noise.data());
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture->setWrapMode(QOpenGLTexture::Repeat);
    texture->generateMipMaps();
    return texture; 
}

std::vector<QVector3D> AmbientOcclusion::genSSAOKernel() {
    std::vector<QVector3D>  kernel(MAX_KERNEL_SIZE);
	for (uint i = 0 ; i < MAX_KERNEL_SIZE ; i++ ) {
        float scale = (float)i / (float)(MAX_KERNEL_SIZE);        
        QVector3D v;
        v.setX(2.0f * (float)rand()/RAND_MAX - 1.0f);
        v.setY(2.0f * (float)rand()/RAND_MAX - 1.0f);
        v.setZ(2.0f * (float)rand()/RAND_MAX - 1.0f);
        v *= (0.1f + 0.9f * scale * scale);
        kernel[i] = v;
    }
	return kernel;
}

std::shared_ptr<QOpenGLShaderProgram> AmbientOcclusion::getShader() {
    return program_;
}

void AmbientOcclusion::initUniformValues() {
	kernelUniform_ = program_->uniformLocation("kernel");
    projMatrixUniform_ = program_->uniformLocation("projection");
	aspectUniform_ = program_->uniformLocation("aspectRatio");
    tanHalfFOVUniform_ = program_->uniformLocation("tanHalfFOV");
    kernelSizeUniform_ = program_->uniformLocation("KERNEL_SIZE");
    hasAOUniform_ = program_->uniformLocation("hasAO");
    sampleRadUniform_ = program_->uniformLocation("sampleRadius");
    screenSizeUniform_ = program_->uniformLocation("screenSize");
}

void AmbientOcclusion::setUniformValues() {
    QMatrix4x4 projMatrix = data_->currentCamera->getProjection();
    float tanHalfFOV = std::tan(data_->currentCamera->FOV()*3.14/360);
    float aspectRatio = ((float)data_->size.width())/data_->size.height();

    program_->setUniformValueArray(kernelUniform_, SSAOKernel.data(), MAX_KERNEL_SIZE);
    program_->setUniformValue(projMatrixUniform_, projMatrix);
	program_->setUniformValue(aspectUniform_, aspectRatio);
    program_->setUniformValue(tanHalfFOVUniform_, tanHalfFOV);

	program_->setUniformValue("depthMap", DEPTH_UNIT_INDEX);
    program_->setUniformValue("noiseMap", NOISE_UNIT_INDEX);
    program_->setUniformValue("positionMap", POSITION_UNIT_INDEX);
	program_->setUniformValue("normalMap", NORMAL_UNIT_INDEX);

    program_->setUniformValue(kernelSizeUniform_, data_->kernelSize);
    program_->setUniformValue(hasAOUniform_, data_->hasAO);
    program_->setUniformValue(sampleRadUniform_, data_->sampleRadius);

    program_->setUniformValue(screenSizeUniform_, data_->size);
}

void AmbientOcclusion::enable() {
	buffer_->write();
    context_->functions()->glClear(GL_COLOR_BUFFER_BIT);
	program_->bind();
	setUniformValues();
}

void AmbientOcclusion::disable() {
    program_->release();
}

void AmbientOcclusion::readBuffer() {
    buffer_->read(1, SSAO_UNIT);
}