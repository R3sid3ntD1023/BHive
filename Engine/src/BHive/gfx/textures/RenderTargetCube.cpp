#include "RenderTargetCube.h"
#include <glad/glad.h>

namespace BHive
{

	RenderTargetCube::RenderTargetCube(uint32_t size, EFormat format)
		: mSize(size)
	{
		/*FTextureCreateInfo create_info{};
		create_info.InternalFormat = format;
		create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		create_info.MinFilter = EMinFilter::LINEAR;
		create_info.GenerateMipMaps = 1;

		mTargetTexture = CreateRef<TextureCube>(size, create_info);

		glCreateFramebuffers(1, &mFramebufferID);
		glNamedFramebufferDrawBuffer(mFramebufferID, GL_COLOR_ATTACHMENT0);

		glCreateRenderbuffers(1, &mRenderBufferID);
		glNamedRenderbufferStorage(mRenderBufferID, GL_DEPTH_COMPONENT24, size, size);
		glNamedFramebufferRenderbuffer(mFramebufferID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferID);

		ASSERT(glCheckNamedFramebufferStatus(mFramebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);*/
	}

	RenderTargetCube::~RenderTargetCube()
	{
		/*glDeleteFramebuffers(1, &mFramebufferID);
		glDeleteRenderbuffers(1, &mRenderBufferID);*/
	}

	void RenderTargetCube::Bind(uint32_t face)
	{
		/*glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glNamedFramebufferTextureLayer(mFramebufferID, GL_COLOR_ATTACHMENT0, mTargetTexture->GetRendererID(), 0, face);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, mSize, mSize);*/
	}

	void RenderTargetCube::UnBind()
	{
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

} // namespace BHive