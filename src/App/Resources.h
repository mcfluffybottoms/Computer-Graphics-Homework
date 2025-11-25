#pragma once
#include <QOpenGLTexture>
#include <QVector2D>
#include <QVector3D>
#include <cstdint>
#include <memory>
#include <vector>


struct Vertex {
	QVector3D position{0, 0, 0};
	QVector3D normal{0, 0, 0};
	QVector2D texCoords{0, 0};
};

struct ModelMesh {
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};
	int32_t textureIndex = -1;
};

struct Model {
	std::vector<ModelMesh> mesh{};
	std::vector<std::unique_ptr<QOpenGLTexture>> texture{};
};
