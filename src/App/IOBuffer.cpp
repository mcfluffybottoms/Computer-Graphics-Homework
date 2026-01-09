#include "IOBuffer.h"
#include <iostream>

IOBuffer::IOBuffer(OpenGLContextPtr context, uint width, uint height, bool hasDepthBuffer)
	: context_(context)
{
	auto * f = context_->functions();
	buffer_ = 0;
	depthTexture_ = 0;

	f->glGenFramebuffers(1, &buffer_);
	f->glBindFramebuffer(GL_FRAMEBUFFER, buffer_);

	if(hasDepthBuffer) {
		f->glGenTextures(1, &depthTexture_);
		f->glBindTexture(GL_TEXTURE_2D, depthTexture_);

		f->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//
		f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//
		f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		f->glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			depthTexture_,
		0);

		GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "FB error, status: " << status << "\n";
			return;
		}
	}

	f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IOBuffer::bindNewColorTexture(uint width, uint height, GLenum internalType) {
	auto * f = context_->functions();

	f->glBindFramebuffer(GL_FRAMEBUFFER, buffer_);

	GLenum format, type;
	switch (internalType)
	{
		case GL_RGB32F:
			format = GL_RGB;
			type = GL_FLOAT;
			break;
		case GL_RGBA16F:
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
		case GL_R32F:
			format = GL_RED;
			type = GL_FLOAT;
			break;
		default:
			std::cerr << "Invalid internal type. Setting to GL_RGBA16F.\n";
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
	}

	textures_.push_back(0);

	f->glGenTextures(1, &textures_.back());
	f->glBindTexture(GL_TEXTURE_2D, textures_.back());

	f->glTexImage2D(GL_TEXTURE_2D, 0, internalType, width, height, 0, format, type, NULL);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	f->glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + textures_.size() - 1,
		GL_TEXTURE_2D,
		textures_.back(),
	0);

	std::vector<GLenum> drawBuffers;
	for (size_t i = 0; i < textures_.size(); ++i)
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);

	f->glDrawBuffers(drawBuffers.size(), drawBuffers.data());

		GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FB error while bindNewColorTexture, status: " << status << "\n";
        return;
    }

}

IOBuffer::~IOBuffer()
{
	auto * f = context_ ? context_->functions() : nullptr;
	if (f == 0)
	{
		return;
	}
	if (buffer_ != 0) {
		f->glDeleteFramebuffers(1, &buffer_);
	}
	if (depthTexture_ != 0) {
		f->glDeleteTextures(1, &depthTexture_);
	}
	if (!textures_.empty()) {
		f->glDeleteTextures(textures_.size(), textures_.data());
	}
    	
}

void IOBuffer::write()
{
	context_->functions()->glBindFramebuffer(GL_FRAMEBUFFER, buffer_);
}

void IOBuffer::read(uint bufferNum, GLenum TextureUnit)
{
	auto * f = context_ ? context_->functions() : nullptr;
	f->glActiveTexture(TextureUnit);
	if(bufferNum == 0) {
		f->glBindTexture(GL_TEXTURE_2D, depthTexture_);
	} else {
		f->glBindTexture(GL_TEXTURE_2D, textures_[bufferNum - 1]);
	}
}