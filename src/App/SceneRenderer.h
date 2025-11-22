#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

struct SceneRenderer {
    SceneRenderer();

    ~SceneRenderer();

private:
    bool setShaders();
    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
	QOpenGLVertexArrayObject vao_;
	std::unique_ptr<QOpenGLShaderProgram> program_;
};