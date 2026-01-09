#include "App/Entity.h"
#include <QOpenGLShaderProgram>
#include "App/IOBuffer.h"
#include "App/SliderGroup.h"
#include <memory>

struct GeometryPass {
    GeometryPass(OpenGLContextPtr context, std::shared_ptr<SharedData> data);

    ~GeometryPass();

    std::shared_ptr<QOpenGLShaderProgram> getShader();

    void initUniformValues();
    void setUniformValues(const QMatrix4x4& transform);
    void enable(const QMatrix4x4 & transform);
    void disable();

    void readDepthBuffer();
    void readPositionBuffer();
    void readNormalBuffer();
    void readAlbedoBuffer();

private:
    OpenGLContextPtr context_;
    std::shared_ptr<SharedData> data_;

    // buffer
    std::unique_ptr<IOBuffer> buffer_;

    // shader info
    std::shared_ptr<QOpenGLShaderProgram> program_;
    GLint projectionUniform_ = -1;
    GLint viewUniform_ = -1;
	GLint modelUniform_ = -1;
	GLint invertedTransposedMatrixUniform_ = -1;
};