#include "IOBuffer.h"

IOBuffer::IOBuffer(OpenGLContextPtr context, uint width, uint height, GLenum internalType)
	: context_(context)
	, textureType_(internalType == GL_NONE ? GL_DEPTH_COMPONENT32F : internalType)
{
	auto * f = context_ ? context_->functions() : nullptr;
	buffer_ = 0;
	texture_ = 0;

	GLenum format, type;
	switch (internalType)
	{
		case GL_RGB32F:
			format = GL_RGB;
			type = GL_FLOAT;
			break;
		case GL_R32F:
			format = GL_RED;
			type = GL_FLOAT;
			break;
		case GL_NONE:
			format = GL_DEPTH_COMPONENT;
			type = GL_FLOAT;
			break;
		default:
			std::cerr << "Invalid internal type. Setting to GL_NONE.\n";
			format = GL_DEPTH_COMPONENT;
			type = GL_FLOAT;
			break;
	}

	f->glGenFramebuffers(1, &buffer_);
	f->glBindFramebuffer(GL_FRAMEBUFFER, buffer_);

	f->glGenTextures(1, &texture_);
	f->glBindTexture(GL_TEXTURE_2D, texture_);


	f->glTexImage2D(GL_TEXTURE_2D, 0, textureType_, width, height, 0, format, type, NULL);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	if (internalType == GL_NONE)
	{
		// DEBUG
		f->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		f->glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		//DEBUG
		f->glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			texture_,
			0);


    	f->glDrawBuffer(GL_NONE);
    	f->glReadBuffer(GL_NONE);
	}
	else
	{
		f->glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			texture_,
			0);

		GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
		f->glDrawBuffers(1, drawBuffers);
	}

    GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FB error, status: " << status << "\n";
        return;
    }

	f->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

IOBuffer::~IOBuffer()
{
	auto * f = context_ ? context_->functions() : nullptr;
	if (f == 0)
	{
		return;
	}
	if (buffer_ != 0)
	{
		f->glDeleteFramebuffers(1, &buffer_);
	}

	if (texture_ != 0)
	{
		f->glDeleteTextures(1, &texture_);
	}
}


void IOBuffer::write()
{
	context_->functions()->glBindFramebuffer(GL_FRAMEBUFFER, buffer_);
}

void IOBuffer::read(GLenum TextureUnit)
{
	auto * f = context_ ? context_->functions() : nullptr;
	f->glActiveTexture(TextureUnit);
	f->glBindTexture(GL_TEXTURE_2D, texture_);
}