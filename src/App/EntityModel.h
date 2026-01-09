#pragma once

#include "App/Entity.h"
#include "OpenGLContext.h"
#include "Resources.h"
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <tinygltf/tiny_gltf.h>


struct EntityModel : public Entity {
	EntityModel();

	bool setShaders();

	bool setImportedModel(const QString & source);
	Model * getImportedModel();

	bool loadBuffers(std::shared_ptr<QOpenGLShaderProgram> program_);

	bool renderMesh(OpenGLContextPtr context_);
	bool render(OpenGLContextPtr context_, GLint texIndex);

	~EntityModel();

private:
	Model * getMesh(const QString & source);
	std::vector<std::unique_ptr<QOpenGLTexture>> loadTextures(const tinygltf::Model & model);
	std::vector<ModelMesh> loadMeshes(const tinygltf::Model & model);
	Model * importedModel = nullptr;

	std::vector<std::unique_ptr<QOpenGLBuffer>> vbos_;
	std::vector<std::unique_ptr<QOpenGLBuffer>> ibos_;
	std::vector<std::unique_ptr<QOpenGLVertexArrayObject>> vaos_;
};
