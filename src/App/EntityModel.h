#pragma once

#include <tinygltf/tiny_gltf.h>
#include "App/Entity.h"
#include "Resources.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include "OpenGLContext.h"

struct EntityModel : public Entity {
    EntityModel(std::shared_ptr<QOpenGLShaderProgram> program);

    bool setShaders();
    void setUniformValues(const QMatrix4x4 & mvp);

    bool setImportedModel(const QString& source);
    Model* getImportedModel();

    bool render(const QMatrix4x4 & mvp, OpenGLContextPtr context_);

    ~EntityModel();

    GLint mvpUniform_ = -1;
    GLint modelUniform_ = -1;
    GLint normalMatrixUniform_ = -1;
private:
    Model* getMesh(const QString& source);
    std::vector<std::unique_ptr<QOpenGLTexture>> loadTextures(const tinygltf::Model& model);
    std::vector<ModelMesh> loadMeshes(const tinygltf::Model& model);
    bool loadBuffers();
    Model* importedModel = nullptr;

    std::vector<std::unique_ptr<QOpenGLBuffer>> vbos_;
	std::vector<std::unique_ptr<QOpenGLBuffer>> ibos_;
	std::vector<std::unique_ptr<QOpenGLVertexArrayObject>> vaos_;
	std::shared_ptr<QOpenGLShaderProgram> program_;
};
