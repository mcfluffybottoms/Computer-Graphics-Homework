#pragma once

#include "App/Entity.h"
#include "OpenGLContext.h"
#include "Resources.h"
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <tinygltf/tiny_gltf.h>


struct EntityModel : public Entity {
	EntityModel(std::shared_ptr<QOpenGLShaderProgram> program1, std::shared_ptr<QOpenGLShaderProgram> program2, std::shared_ptr<QOpenGLShaderProgram> program3, std::shared_ptr<QOpenGLShaderProgram> program4);

	bool setShaders();
	void setUniformValues(const QMatrix4x4 & mvp);

	bool setImportedModel(const QString & source);
	Model * getImportedModel();

	bool render(const QMatrix4x4 & mvp, OpenGLContextPtr context_);

	~EntityModel();

	GLint mvpUniform_ = -1;
	GLint modelUniform_ = -1;
	GLint invertedTransposedMatrixUniform_ = -1;

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
	std::shared_ptr<QOpenGLShaderProgram> m_geometry_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_ao_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_blur_program_;
	std::shared_ptr<QOpenGLShaderProgram> m_lighting_program_;
};
