#pragma once

#include <tinygltf/tiny_gltf.h>
#include "Resources.h"

struct ResourceManager {
    ResourceManager();
    bool setImportedModel(const QString& source);
    ~ResourceManager();
private:
    Model* getMesh(const QString& source);
    std::vector<std::unique_ptr<QOpenGLTexture>> loadTextures(const tinygltf::Model& model);
    std::vector<ModelMesh> loadMeshes(const tinygltf::Model& model);
    Model* importedModel = nullptr;
};