#pragma once

#include "App/Entity.h"
#include "App/OpenGLContext.h"
#include <vector>

struct IOBuffer {
    IOBuffer(OpenGLContextPtr context, uint width, uint height, bool hasDepthBuffer);

    void bindNewColorTexture(uint width, uint height, GLenum internalType);

    ~IOBuffer();

    void write();

    void read(uint bufferNum, GLenum TextureUnit);

private:
    OpenGLContextPtr context_;
	GLuint buffer_;

    std::vector<GLuint> textures_;
    GLuint depthTexture_;
};