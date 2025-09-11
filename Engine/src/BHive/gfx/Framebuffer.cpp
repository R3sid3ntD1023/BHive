#include "Framebuffer.h"
#include "textures/Texture2D.h"
#include "textures/Texture2DArray.h"
#include "textures/Texture2DMultisample.h"
#include "textures/TextureCube.h"
#include "textures/TextureCubeArray.h"
#include "utils/texture/TextureUtils.h"
#include <glad/glad.h>

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	Ref<Texture> CreateFramebufferTexture(uint32_t w, uint32_t h, uint32_t d, uint32_t samples, FFramebufferTexture specification)
	{
		auto &type = specification.TextureType;

		if (samples > 1 && type == ETextureType::TEXTURE_2D)
			return CreateRef<Texture2DMultisample>(w, h, samples, specification.CreateInfo);

		switch (type)
		{
		case ETextureType::TEXTURE_2D:
			return CreateRef<Texture2D>(w, h, specification.CreateInfo);
		case ETextureType::TEXTURE_CUBE_MAP:
			return CreateRef<TextureCube>(w, specification.CreateInfo);
		case ETextureType::TEXTURE_2D_ARRAY:
			return CreateRef<Texture2DArray>(w, h, d, specification.CreateInfo);
		case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
			return CreateRef<TextureCubeArray>(w, h, d, specification.CreateInfo);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Framebuffer::Framebuffer(const FramebufferSpecification &specification)
		: mSpecification(specification)
	{
		for (auto &spec : mSpecification.Attachments.GetAttachments())
		{
			if (TextureUtils::IsDepthFormat(spec.CreateInfo.InternalFormat))
			{
				mDepthSpecification = spec;
				mDepthAPIInfo = spec.CreateInfo;
				continue;
			}

			mColorAttachmentSpecifications.emplace_back(spec);
			mColorAttachmentAPIInfos.emplace_back(spec.CreateInfo);
		}

		mRenderBufferSpecification = mSpecification.Attachments.GetRenderBuffer();

		Initialize();
	}

	Framebuffer::~Framebuffer()
	{
		Release();
	}

	void Framebuffer::Bind() const
	{

		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glViewport(0, 0, mSpecification.Width, mSpecification.Height);
	}

	void Framebuffer::UnBind() const
	{

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
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

	void Framebuffer::ClearAttachment(uint32_t attachmentIndex, const int *data)
	{
		ASSERT(attachmentIndex < mColorAttachments.size());

		glClearNamedFramebufferiv(mFramebufferID, GL_COLOR, attachmentIndex, data);
	}

	void Framebuffer::ClearAttachment(uint32_t attachmentIndex, const float *data)
	{
		glClearNamedFramebufferfv(mFramebufferID, GL_COLOR, attachmentIndex, data);
	}

	void Framebuffer::Blit(const Ref<Framebuffer> &target)
	{
		if (!target || mSpecification.Width == 0 || mSpecification.Height == 0)
			return;

		const auto &specs = mSpecification;

		if (!target || specs.Width == 0 || specs.Height == 0)
			return;

		const auto &dst_specs = target->GetSpecification();
		auto count = dst_specs.Attachments.GetAttachments().size();

		const auto read_target = mFramebufferID;
		const auto draw_target = target ? target->GetRendererID() : 0;

		for (size_t i = 0; i < count; i++)
		{
			glNamedFramebufferReadBuffer(read_target, GL_COLOR_ATTACHMENT0 + i);
			glNamedFramebufferDrawBuffer(draw_target, GL_COLOR_ATTACHMENT0 + i);

			glBlitNamedFramebuffer(read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		if (target->GetDepthAttachment() && GetDepthAttachment())
		{
			glBlitNamedFramebuffer(read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		}
	}

	void Framebuffer::BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h)
	{
		const auto &specs = mSpecification;

		if (specs.Width == 0 || specs.Height == 0)
			return;

		const auto read_target = mFramebufferID;
		const auto draw_target = 0;

		glNamedFramebufferReadBuffer(read_target, GL_COLOR_ATTACHMENT0);

		glBlitNamedFramebuffer(read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void Framebuffer::ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const
	{
		ASSERT(attachmentIndex < mColorAttachmentSpecifications.size());

		auto &spec = mColorAttachmentAPIInfos[attachmentIndex];

		glNamedFramebufferReadBuffer(mFramebufferID, GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glReadPixels(x, y, w, h, spec.Format, spec.Type, data);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	Ref<Texture> Framebuffer::GetColorAttachment(uint32_t index) const
	{
		ASSERT(index < mColorAttachments.size());
		return mColorAttachments[index];
	}

	Ref<Texture> Framebuffer::GetDepthAttachment() const
	{
		return mDepthAttachment;
	}

	void Framebuffer::Release()
	{
		glDeleteRenderbuffers(1, &mRenderbufferID);
		glDeleteFramebuffers(1, &mFramebufferID);
	}

	void Framebuffer::Initialize()
	{
		if (mFramebufferID)
		{
			mColorAttachments.clear();
			mDepthAttachment.reset();

			Release();
		}

		auto numColorAttachments = mColorAttachmentSpecifications.size();
		if (numColorAttachments)
		{
			mColorAttachments.resize(numColorAttachments);
			for (size_t i = 0; i < numColorAttachments; i++)
			{
				auto &specs = mColorAttachmentSpecifications[i];
				auto &attachment = mColorAttachments[i];

				attachment = CreateFramebufferTexture(mSpecification.Width, mSpecification.Height, mSpecification.Depth, mSpecification.Samples, specs);
			}
		}

		if (mDepthSpecification.CreateInfo.InternalFormat != EFormat::Invalid)
		{
			mDepthAttachment = CreateFramebufferTexture(mSpecification.Width, mSpecification.Height, mSpecification.Depth, mSpecification.Samples, mDepthSpecification);
		}

		// create framebuffer and attach textures
		glCreateFramebuffers(1, &mFramebufferID);
		auto num_attachments = mColorAttachments.size();
		for (size_t i = 0; i < num_attachments; i++)
		{
			glNamedFramebufferTexture(mFramebufferID, GL_COLOR_ATTACHMENT0 + (uint32_t)i, *mColorAttachments[i], 0);
		}

		if (mDepthAttachment)
		{
			glNamedFramebufferTexture(mFramebufferID, TextureUtils::GetAPIDepthAttachmentType(mDepthSpecification.CreateInfo.InternalFormat), *mDepthAttachment, 0);
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

		if (mRenderBufferSpecification.Format != EFormat::Invalid && !mDepthAttachment)
		{
			auto depth_attachment = TextureUtils::GetAPIDepthAttachmentType(mRenderBufferSpecification.Format);
			auto depth_internal_format = TextureUtils::GetAPIInternalFormat(mRenderBufferSpecification.Format);

			glCreateRenderbuffers(1, &mRenderbufferID);
			glNamedRenderbufferStorage(mRenderbufferID, depth_internal_format, mSpecification.Width, mSpecification.Height);
			glNamedFramebufferRenderbuffer(mFramebufferID, depth_attachment, GL_RENDERBUFFER, mRenderbufferID);
		}

		ASSERT(glCheckNamedFramebufferStatus(mFramebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

} // namespace BHive