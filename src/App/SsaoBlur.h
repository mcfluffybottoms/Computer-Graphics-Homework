#include "App/Entity.h"
#include <QOpenGLShaderProgram>
#include "App/IOBuffer.h"
#include "App/SliderGroup.h"
#include <memory>

struct Blur {
    Blur(OpenGLContextPtr context, std::shared_ptr<SharedData> data);

    ~Blur();

    std::shared_ptr<QOpenGLShaderProgram> getShader();

    void initUniformValues();
    void setUniformValues();
    void enable();
    void disable();

    void readBuffer();

private:
    OpenGLContextPtr context_;
    std::shared_ptr<SharedData> data_;

    // buffer
    std::unique_ptr<IOBuffer> buffer_;

    // shader info
    std::shared_ptr<QOpenGLShaderProgram> program_;
    GLuint samplesUniform_ = -1;
    GLuint hasBlurUniform_ = -1;
};