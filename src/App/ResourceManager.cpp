#include "ResourceManager.h"
#include "App/Resources.h"
#include <QFile>
#include <iostream>
#include <tinygltf/tiny_gltf.h>

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager()
{
	if (importedModel)
		delete importedModel;
}

bool ResourceManager::setImportedModel(const QString & source)
{
	Model * model = getMesh(source);
	if (!model)
	{
		std::cerr << "Model not set." << std::endl;
		return false;
	}
	importedModel = model;
	return true;
}

Model * ResourceManager::getMesh(const QString & source)
{
	QFile modelFile(source);
	if (!modelFile.open(QIODevice::ReadOnly))
	{
		std::cerr << "ResourceManager::getMesh: " << "File was not opened." << std::endl;
		return nullptr;
	}

	QByteArray modelData = modelFile.readAll();
	modelFile.close();

	if (modelData.isEmpty())
	{
		std::cerr << "ResourceManager::getMesh: " << "No data in file." << std::endl;
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
		std::cerr << "ResourceManager::getMesh: " << warning << "; " << err << std::endl;
		return nullptr;
	}

	auto textures = loadTextures(model);
	auto meshes = loadMeshes(model);
	Model * endModel = new Model{std::move(meshes), std::move(textures)};

	return endModel;
}

std::vector<std::unique_ptr<QOpenGLTexture>> ResourceManager::loadTextures(const tinygltf::Model & model)
{
	std::vector<std::unique_ptr<QOpenGLTexture>> textures_;
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
			QImage qimg = QImage(image.image.data(), image.width, image.height, fmt);

			auto tex = std::make_unique<QOpenGLTexture>(qimg);
			tex->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
			tex->setWrapMode(QOpenGLTexture::Repeat);
			tex->generateMipMaps();

			textures_.push_back(std::move(tex));
		}
	}
	return textures_;
}

std::vector<ModelMesh> ResourceManager::loadMeshes(const tinygltf::Model & model)
{
	std::vector<ModelMesh> meshes_;
	for (const auto & mesh: model.meshes)
	{
		for (const auto & primitive: mesh.primitives)
		{
			ModelMesh meshData;

			// filling vertices
			for (const auto & attributeName: {"POSITION", "NORMAL", "TEXCOORD_0"})
			{
				const auto & accessor = model.accessors[primitive.attributes.at(attributeName)];
				const auto & bufferView = model.bufferViews[accessor.bufferView];
				const auto & buffer = model.buffers[bufferView.buffer];

				const float * values = reinterpret_cast<const float *>(
					&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

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