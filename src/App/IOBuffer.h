#include "App/Entity.h"
#include "App/OpenGLContext.h"
#include <GL/gl.h>
#include <iostream>

struct IOBuffer {
    IOBuffer(OpenGLContextPtr context, uint width, uint height, GLenum internalType);

    ~IOBuffer();

    void write();

    void read(GLenum TextureUnit);

private:
    OpenGLContextPtr context_;
    GLenum textureType_;
	GLuint buffer_;
	GLuint texture_ = 0;

    GLuint debug_ = 0;
};