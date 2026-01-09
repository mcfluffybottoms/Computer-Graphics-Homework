#include "App/Skybox.h"
#include "App/Camera.h"
#include "App/EntityModel.h"
#include "App/OpenGLContext.h"
#include <qvector3d.h>

Skybox::Skybox(OpenGLContextPtr context, std::shared_ptr<SharedData> data) : context_(context), data_(data) {
    program_ = std::make_shared<QOpenGLShaderProgram>();
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/skybox.vs");
	program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
											   ":/Shaders/skybox.fs");
	program_->link();

    viewUniform_= program_->uniformLocation("view");
	projUniform_= program_->uniformLocation("projection");
    transformUniform_= program_->uniformLocation("transform");
}
Skybox::~Skybox() {
    program_.reset();
    if(model) delete model;
}
void Skybox::init() {
    model = new EntityModel();
    model->setImportedModel(":/Models/cube.glb");
	model->loadBuffers(program_);
    model->setScale(QVector3D(1.0f, 1.0f, 1.0f));

    context_->functions()->glGenVertexArrays(1, &quadVAO);
	context_->functions()->glGenBuffers(1, &quadVBO);
	context_->functions()->glBindVertexArray(quadVAO);
	context_->functions()->glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	context_->functions()->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	context_->functions()->glEnableVertexAttribArray(0);
	context_->functions()->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    context_->functions()->glEnableVertexAttribArray(1);
    context_->functions()->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    context_->functions()->glEnableVertexAttribArray(2);
    context_->functions()->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    context_->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    context_->functions()->glBindVertexArray(0);
    loadTexture();
}
void Skybox::render(Camera* camera) {
    context_->functions()->glDepthFunc(GL_LEQUAL); 

    //model->setPosition(camera->position());
    program_->bind();
    program_->setUniformValue(viewUniform_, camera->getView());
    program_->setUniformValue(projUniform_, camera->getProjection());
    program_->setUniformValue(transformUniform_, model->getTransform());
    program_->setUniformValue("skybox", SKYBOX_UNIT_INDEX);
    
    context_->functions()->glBindVertexArray(quadVAO);
    context_->functions()->glDrawArrays(GL_TRIANGLES, 0, 36);
    context_->functions()->glBindVertexArray(0);
    
    program_->release();
    context_->functions()->glDepthFunc(GL_LESS); 
}

void Skybox::loadTexture() {
    QStringList faceFiles = {
        ":/Textures/right.jpg",
        ":/Textures/left.jpg",
        ":/Textures/top.jpg",
        ":/Textures/bottom.jpg",
        ":/Textures/front.jpg",
        ":/Textures/back.jpg"
    };

    cubemap_ = std::make_unique<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);
    cubemap_->create();
    cubemap_->bind();

    QImage first(faceFiles[0]);
    cubemap_->setSize(first.width(), first.height());
    cubemap_->setFormat(QOpenGLTexture::RGBA8_UNorm);
    cubemap_->allocateStorage();

    for (int i = 0; i < 6; ++i) {
        QImage img(faceFiles[i]);
        img = img.convertToFormat(QImage::Format_RGBA8888);
        cubemap_->setData(
            0, 0,
            static_cast<QOpenGLTexture::CubeMapFace>(
                QOpenGLTexture::CubeMapPositiveX + i),
            QOpenGLTexture::RGBA,
            QOpenGLTexture::UInt8,
            img.constBits()
        );
    }

    cubemap_->setWrapMode(QOpenGLTexture::ClampToEdge);
    cubemap_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    cubemap_->setMagnificationFilter(QOpenGLTexture::Linear);
    cubemap_->generateMipMaps();

    cubemap_->bind(SKYBOX_UNIT_INDEX);
}