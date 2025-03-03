#include "GLFramebuffer.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>
#include "gfx/Texture.h"
#include "utils/ImageUtils.h"

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	GLFramebuffer::GLFramebuffer(const FramebufferSpecification &specification)
		: mSpecification(specification)
	{
		for (auto &spec : mSpecification.Attachments.GetAttachments())
		{
			if (IsDepthFormat(spec.mSpecification.mFormat))
			{
				mDepthSpecification = spec;
				continue;
			}

			mColorSpecifications.push_back(spec);
		}

		Initialize();
	}

	GLFramebuffer::~GLFramebuffer()
	{
		Release();
	}

	void GLFramebuffer::Bind() const
	{

		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glViewport(0, 0, mSpecification.Width, mSpecification.Height);
	}

	void GLFramebuffer::UnBind() const
	{

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > sMaxFramebufferSize || height > sMaxFramebufferSize)
		{
			LOG_WARN("Attempted to resize framebuffer to <{},{}>", width, height);
			return;
		}

		mSpecification.Width = width;
		mSpecification.Height = height;

		Initialize();
	}

	void GLFramebuffer::ClearAttachment(uint32_t attachmentIndex, unsigned type, const float *data)
	{
		ASSERT(attachmentIndex < mColorAttachments.size());

		auto &attachment = mColorAttachments[attachmentIndex];
		auto &spec = mColorSpecifications[attachmentIndex];

		glClearNamedFramebufferfv(mFramebufferID, GL_COLOR, attachmentIndex, data);
	}

	void GLFramebuffer::Blit(Ref<Framebuffer> &target)
	{
		if (!target || mSpecification.Width == 0 || mSpecification.Height == 0)
			return;

		const auto &specs = mSpecification;

		if (!target || specs.Width == 0 || specs.Height == 0)
			return;

		const auto &dst_specs = target->GetSpecification();
		auto count = dst_specs.Attachments.GetAttachments().size();

		const auto read_target = mFramebufferID;
		const auto draw_target = target->GetRendererID();

		for (size_t i = 0; i < count; i++)
		{
			glNamedFramebufferReadBuffer(read_target, GL_COLOR_ATTACHMENT0 + i);
			glNamedFramebufferDrawBuffer(draw_target, GL_COLOR_ATTACHMENT0 + i);

			glBlitNamedFramebuffer(
				read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		if (target->GetDepthAttachment() && GetDepthAttachment())
		{
			glBlitNamedFramebuffer(
				read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height,
				GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		}
	}

	void GLFramebuffer::ReadPixel(
		uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data) const
	{
		ASSERT(attachmentIndex < mColorSpecifications.size());

		auto &spec = mColorSpecifications[attachmentIndex];

		glNamedFramebufferReadBuffer(mFramebufferID, GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glReadPixels(x, y, w, h, GetGLFormat(spec.mSpecification.mFormat), type, data);
	}

	void GLFramebuffer::Release()
	{

		glDeleteFramebuffers(1, &mFramebufferID);
	}

	void GLFramebuffer::Initialize()
	{
		if (mFramebufferID)
		{
			mColorAttachments.clear();
			mDepthAttachment.reset();
			Release();
		}

		auto numColorAttachments = mColorSpecifications.size();
		if (numColorAttachments)
		{
			mColorAttachments.resize(numColorAttachments);
			for (size_t i = 0; i < numColorAttachments; i++)
			{
				auto &specification = mColorSpecifications[i];
				auto &attachment = mColorAttachments[i];

				switch (specification.TextureType)
				{
				case ETextureType::TEXTURE_2D:
					attachment = Texture2D::Create(
						mSpecification.Width, mSpecification.Height, specification.mSpecification, mSpecification.Samples);
					break;
				case ETextureType::TEXTURE_CUBE_MAP:
					attachment = TextureCube::Create(mSpecification.Width, specification.mSpecification);
					break;
				case ETextureType::TEXTURE_3D:
					attachment = Texture3D::Create(
						mSpecification.Width, mSpecification.Height, mSpecification.Depth, specification.mSpecification);
					break;
				default:
					break;
				}
			}
		}

		if (mDepthSpecification.mSpecification.mFormat != EFormat::Invalid)
		{
			switch (mDepthSpecification.TextureType)
			{
			case ETextureType::TEXTURE_2D:
				mDepthAttachment = Texture2D::Create(
					mSpecification.Width, mSpecification.Height, mDepthSpecification.mSpecification, mSpecification.Samples);
				break;
			case ETextureType::TEXTURE_CUBE_MAP:
				mDepthAttachment = TextureCube::Create(mSpecification.Width, mDepthSpecification.mSpecification);
				break;
			case ETextureType::TEXTURE_3D:
				mDepthAttachment = Texture3D::Create(
					mSpecification.Width, mSpecification.Height, mSpecification.Depth, mDepthSpecification.mSpecification);
				break;
			default:
				break;
			}
		}

		glCreateFramebuffers(1, &mFramebufferID);
		auto num_attachments = mColorAttachments.size();
		for (size_t i = 0; i < num_attachments; i++)
		{
			glNamedFramebufferTexture(mFramebufferID, GL_COLOR_ATTACHMENT0 + (uint32_t)i, *mColorAttachments[i], 0);
		}

		if (mDepthAttachment)
		{
			glNamedFramebufferTexture(
				mFramebufferID, GetDepthAttachmentType(mDepthSpecification.mSpecification.mFormat), *mDepthAttachment, 0);
		}

		if (num_attachments > 1)
		{
			std::vector<GLenum> buffers(num_attachments);
			for (size_t i = 0; i < num_attachments; i++)
			{
				buffers[i] = GL_COLOR_ATTACHMENT0 + (int32_t)i;
			}
			glNamedFramebufferDrawBuffers(mFramebufferID, (uint32_t)buffers.size(), buffers.data());
		}
		else if (num_attachments == 0)
		{
			glNamedFramebufferDrawBuffer(mFramebufferID, GL_NONE);
		}

		if (mSpecification.WriteOnly)
		{
			auto &specification = mSpecification.mRenderSpecification;
			glCreateRenderbuffers(1, &mRenderBufferID);
			glNamedRenderbufferStorage(
				mRenderBufferID, GetGLInternalFormat(specification.mSpecification.mFormat), mSpecification.Width,
				mSpecification.Height);
			glNamedFramebufferRenderbuffer(
				mFramebufferID, GetDepthAttachmentType(specification.mSpecification.mFormat), GL_RENDERBUFFER,
				mRenderBufferID);
		}

		ASSERT(glCheckNamedFramebufferStatus(mFramebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

} // namespace BHive