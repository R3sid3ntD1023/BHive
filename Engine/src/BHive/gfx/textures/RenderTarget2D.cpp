#include "RenderTarget2D.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>

namespace BHive
{
	RenderTarget2D::RenderTarget2D(const Ref<Texture> &texture, uint32_t attachment, uint32_t level)
		: mTargetTexture(texture)
	{
		if (!texture)
			return;

		mWidth = texture->GetWidth();
		mHeight = texture->GetHeight();

		glCreateFramebuffers(1, &mFramebufferID);
		glNamedFramebufferTexture(mFramebufferID, GL_COLOR_ATTACHMENT0 + attachment, texture->GetRendererID(), level);

		glCreateRenderbuffers(1, &mRenderBufferID);
		glNamedRenderbufferStorage(mRenderBufferID, GL_DEPTH_COMPONENT, mWidth, mHeight);
		glNamedFramebufferRenderbuffer(mFramebufferID, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, mRenderBufferID);

		ASSERT(glCheckNamedFramebufferStatus(GL_FRAMEBUFFER, mFramebufferID) == GL_FRAMEBUFFER_COMPLETE);
	}

	RenderTarget2D::~RenderTarget2D()
	{
		glDeleteFramebuffers(1, &mFramebufferID);
		glDeleteRenderbuffers(1, &mRenderBufferID);
	}

	void RenderTarget2D::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glViewport(0, 0, mWidth, mHeight);
	}

	void RenderTarget2D::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

} // namespace BHive