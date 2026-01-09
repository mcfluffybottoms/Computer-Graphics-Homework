#include "EntityModel.h"
#include "App/Resources.h"
#include <QFile>
#include <iostream>
#include <qimage.h>
#include <tinygltf/tiny_gltf.h>

EntityModel::EntityModel()
{}

EntityModel::~EntityModel()
{
	if (importedModel)
		delete importedModel;
}

bool EntityModel::setImportedModel(const QString & source)
{
	Model * model = getMesh(source);
	if (!model)
	{
		std::cerr << "Model not set." << std::endl;
		return false;
	}
	importedModel = model;
	//loadBuffers();
	return true;
}

Model * EntityModel::getImportedModel()
{
	return importedModel;
}

Model * EntityModel::getMesh(const QString & source)
{
	QFile modelFile(source);
	if (!modelFile.open(QIODevice::ReadOnly))
	{
		std::cerr << "EntityModel::getMesh: " << "File was not opened." << std::endl;
		return nullptr;
	}

	QByteArray modelData = modelFile.readAll();
	modelFile.close();

	if (modelData.isEmpty())
	{
		std::cerr << "EntityModel::getMesh: " << "No data in file." << std::endl;
		return nullptr;
	}

	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err, warning;

	bool success = loader.LoadBinaryFromMemory(&model, &err, &warning,
											   reinterpret_cast<const unsigned char *>(modelData.data()),
											   static_cast<unsigned int>(modelData.size()));

	if (!success)
	{
		std::cerr << "EntityModel::getMesh: " << warning << "; " << err << std::endl;
		return nullptr;
	}

	auto textures = loadTextures(model);
	//auto textures = std::vector<std::unique_ptr<QOpenGLTexture>>();
	auto meshes = loadMeshes(model);
	Model * endModel = new Model{std::move(meshes), std::move(textures)};

	return endModel;
}

std::vector<std::unique_ptr<QOpenGLTexture>> EntityModel::loadTextures(const tinygltf::Model & model)
{
	std::vector<std::unique_ptr<QOpenGLTexture>> textures_;
	//textures_.push_back(std::make_unique<QOpenGLTexture>(QImage(":/Textures/voronoi.png")));// fallback texture
	for (const auto & texture: model.textures)
	{
		if (texture.source >= 0 && texture.source < static_cast<int>(model.images.size()))
		{
			const auto & image = model.images[texture.source];

			QImage::Format fmt;
			if (image.component == 1)
			{
				fmt = QImage::Format_Mono;
			}
			else if (image.component == 2)
			{
				fmt = QImage::Format_RGB16;
			}
			else if (image.component == 3)
			{
				fmt = QImage::Format_RGB888;
			}
			else if (image.component == 4)
			{
				fmt = QImage::Format_RGBA8888;
			}
			else
			{
				fmt = QImage::Format_Invalid;
			}
			QImage qimg = QImage(image.image.data(), image.width, image.height, fmt);

			auto tex = std::make_unique<QOpenGLTexture>(qimg);
			tex->create();
    		tex->bind();
			tex->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
			tex->setWrapMode(QOpenGLTexture::Repeat);
			tex->setMagnificationFilter(QOpenGLTexture::Linear);
			tex->generateMipMaps();

			textures_.push_back(std::move(tex));
		}
	}

	return textures_;
}

std::vector<ModelMesh> EntityModel::loadMeshes(const tinygltf::Model & model)
{
	std::vector<ModelMesh> meshes_;
	for (const auto & mesh: model.meshes)
	{
		for (const auto & primitive: mesh.primitives)
		{
			ModelMesh meshData;

			for (const auto & attributeName: {"POSITION", "NORMAL", "TEXCOORD_0"})
			{
				if (primitive.attributes.count(attributeName) == 0)
					continue;
				const auto & accessor = model.accessors[primitive.attributes.at(attributeName)];
				const auto & bufferView = model.bufferViews[accessor.bufferView];
				const auto & buffer = model.buffers[bufferView.buffer];

				const float * values = reinterpret_cast<const float *>(
					&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

				if (accessor.count > meshData.vertices.size())
					meshData.vertices.resize(accessor.count);
				if (strcmp(attributeName, "POSITION") == 0)
				{
					for (size_t i = 0; i < accessor.count; ++i)
					{
						meshData.vertices[i].position[0] = values[i * 3 + 0];
						meshData.vertices[i].position[1] = values[i * 3 + 1];
						meshData.vertices[i].position[2] = values[i * 3 + 2];
					}
				}
				else if (strcmp(attributeName, "NORMAL") == 0)
				{
					for (size_t i = 0; i < accessor.count; ++i)
					{
						meshData.vertices[i].normal[0] = values[i * 3 + 0];
						meshData.vertices[i].normal[1] = values[i * 3 + 1];
						meshData.vertices[i].normal[2] = values[i * 3 + 2];
					}
				}
				else if (strcmp(attributeName, "TEXCOORD_0") == 0)
				{
					for (size_t i = 0; i < accessor.count; ++i)
					{
						meshData.vertices[i].texCoords[0] = values[i * 2 + 0];
						meshData.vertices[i].texCoords[1] = values[i * 2 + 1];
					}
				}
			}

			// filling indices
			if (primitive.indices >= 0)
			{
				const auto & accessor = model.accessors[primitive.indices];
				const auto & bufferView = model.bufferViews[accessor.bufferView];
				const auto & buffer = model.buffers[bufferView.buffer];

				meshData.indices.resize(accessor.count);
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					const uint16_t * indices = reinterpret_cast<const uint16_t *>(
						&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
					for (size_t i = 0; i < accessor.count; ++i)
					{
						meshData.indices[i] = indices[i];
					}
				}
				else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					const uint32_t * indices = reinterpret_cast<const uint32_t *>(
						&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
					for (size_t i = 0; i < accessor.count; ++i)
					{
						meshData.indices[i] = indices[i];
					}
				}
			}

			// finding a texture corresponding
			if (primitive.material >= 0 && primitive.material < static_cast<int>(model.materials.size()))
			{
				const auto & material = model.materials[primitive.material];
				if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
				{
					meshData.textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
				}
			}
			meshes_.push_back(std::move(meshData));
		}
	}
	return meshes_;
}

bool EntityModel::loadBuffers(std::shared_ptr<QOpenGLShaderProgram> program_)
{
	if (!importedModel)
		return false;
	for (const auto & mesh: importedModel->mesh)
	{
		auto vbo_ = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::Type::VertexBuffer);
		auto ibo_ = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::Type::IndexBuffer);
		auto vao_ = std::make_unique<QOpenGLVertexArrayObject>();
		std::unique_ptr<QOpenGLTexture> texture_;

		// Create VAO object
		vao_->create();
		vao_->bind();

		// Create VBO
		vbo_->create();
		vbo_->bind();
		vbo_->setUsagePattern(QOpenGLBuffer::StaticDraw);
		vbo_->allocate(mesh.vertices.data(), static_cast<int>(mesh.vertices.size() * sizeof(Vertex)));

		// Create IBO
		ibo_->create();
		ibo_->bind();
		ibo_->setUsagePattern(QOpenGLBuffer::StaticDraw);
		ibo_->allocate(mesh.indices.data(), static_cast<int>(mesh.indices.size() * sizeof(uint32_t)));

		// Bind attributes
		program_->bind();

		program_->enableAttributeArray(0);
		program_->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));

		program_->enableAttributeArray(1);
		program_->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));

		program_->enableAttributeArray(2);
		program_->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, texCoords), 2, sizeof(Vertex));

		// Release all
		program_->release();

		vaos_.push_back(std::move(vao_));
		vbos_.push_back(std::move(vbo_));
		ibos_.push_back(std::move(ibo_));

		if (mesh.textureIndex >= 0 && mesh.textureIndex < static_cast<int>(importedModel->texture.size()))
		{
			importedModel->texture[mesh.textureIndex]->bind(0);
			importedModel->texture[mesh.textureIndex]->release();
		}
	}

	return true;
}

bool EntityModel::renderMesh(OpenGLContextPtr context)
{
	for (size_t i = 0; i < importedModel->mesh.size(); ++i)
	{
		const auto & mesh = importedModel->mesh[i];
		if (i < vaos_.size() && vaos_[i])
		{
			vaos_[i]->bind();
			context->functions()->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
			vaos_[i]->release();
		}
	}
	return true;
}

bool EntityModel::render(OpenGLContextPtr context, GLint texIndex)
{
	for (size_t i = 0; i < importedModel->mesh.size(); ++i)
	{
		const auto & mesh = importedModel->mesh[i];
		if (i < vaos_.size() && vaos_[i])
		{
			vaos_[i]->bind();

			if (mesh.textureIndex >= 0 && mesh.textureIndex < static_cast<int>(importedModel->texture.size()))
			{
				importedModel->texture[mesh.textureIndex]->bind(texIndex);
			}

			context->functions()->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);

			if (mesh.textureIndex >= 0 && mesh.textureIndex < static_cast<int>(importedModel->texture.size()))
			{
				importedModel->texture[mesh.textureIndex]->release();
			}

			vaos_[i]->release();
		}
	}
	return true;
}