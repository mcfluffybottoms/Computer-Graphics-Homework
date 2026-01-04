#pragma once

#include "App/Entity.h"
#include "OpenGLContext.h"
#include "Resources.h"
#include "ShaderManager.h"
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <tinygltf/tiny_gltf.h>


struct EntityModel : public Entity {
	EntityModel(std::shared_ptr<ShaderManager> program);

	bool setShaders();

	bool setImportedModel(const QString & source);
	Model * getImportedModel();

	bool renderMesh(OpenGLContextPtr context_);
	bool render(std::shared_ptr<QOpenGLShaderProgram> program_, OpenGLContextPtr context_);

	~EntityModel();

private:
	Model * getMesh(const QString & source);
	std::vector<std::unique_ptr<QOpenGLTexture>> loadTextures(const tinygltf::Model & model);
	std::vector<ModelMesh> loadMeshes(const tinygltf::Model & model);
	bool loadBuffers();
	Model * importedModel = nullptr;

	std::vector<std::unique_ptr<QOpenGLBuffer>> vbos_;
	std::vector<std::unique_ptr<QOpenGLBuffer>> ibos_;
	std::vector<std::unique_ptr<QOpenGLVertexArrayObject>> vaos_;

	// shader setup
	std::shared_ptr<ShaderManager> shaderManager;
};
