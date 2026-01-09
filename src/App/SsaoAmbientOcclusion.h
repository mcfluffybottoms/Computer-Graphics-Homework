#include "App/Entity.h"
#include "App/SliderGroup.h"
#include <memory>
#include <QOpenGLShaderProgram>
#include <qopengltexture.h>
#include "App/IOBuffer.h"

struct AmbientOcclusion {
    AmbientOcclusion(OpenGLContextPtr context, std::shared_ptr<SharedData> data);

    ~AmbientOcclusion();

    std::vector<QVector3D> genSSAOKernel();
    std::unique_ptr<QOpenGLTexture> genNoise();

    std::shared_ptr<QOpenGLShaderProgram> getShader();

    void initUniformValues();
    void setUniformValues();

    void enable();
    void disable();

    void readBuffer();

private:
    const uint MAX_KERNEL_SIZE = 512;

    std::vector<QVector3D> SSAOKernel;
    
    OpenGLContextPtr context_;
    std::shared_ptr<SharedData> data_;

    // buffer
    std::unique_ptr<IOBuffer> buffer_;

    // noise texture
    std::unique_ptr<QOpenGLTexture> noise;

    // shader info
    std::shared_ptr<QOpenGLShaderProgram> program_;
    GLuint kernelUniform_ = -1;
    GLint projMatrixUniform_ = -1;
    GLuint aspectUniform_ = -1;
    GLint tanHalfFOVUniform_ = -1;
    GLint depthMapUniform_ = -1;

    GLint screenSizeUniform_ = -1;

    GLint kernelSizeUniform_ = -1;
    GLint hasAOUniform_ = -1;
    GLint sampleRadUniform_ = -1;
    GLint biasUniform_ = -1;
};