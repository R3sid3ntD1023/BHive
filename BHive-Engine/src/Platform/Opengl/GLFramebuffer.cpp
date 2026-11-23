#include "GLFramebuffer.h"
#include "gfx/factories/TextureFactory.h"
#include <glad/glad.h>

namespace BHive
{
	static uint32_t sMaxFramebufferSize = 8192;

	namespace
	{
		void AttachFramebufferTexture(uint32_t framebuffer, GLenum attachment, uint32_t texture, ETextureType type, uint32_t mip, uint32_t layer)
		{
			switch (type)
			{
			case ETextureType::TEXTURE_1D_ARRAY:
			case ETextureType::TEXTURE_2D_ARRAY:
			case ETextureType::TEXTURE_3D:
			case ETextureType::TEXTURE_CUBE:
			case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
				glNamedFramebufferTextureLayer(framebuffer, attachment, texture, mip, layer);
				break;
			default:
				glNamedFramebufferTexture(framebuffer, attachment, texture, mip);
				break;
			}
		}
	} // namespace

	Ref<Texture> CreateTexture(const glm::uvec2 &size, uint32_t samples, FFramebufferTexture specification)
	{
		if (specification.ExternalTexture)
			return specification.ExternalTexture;

		auto &type = specification.Type;
		return TextureFactory::Create(type, size, specification.CreateInfo);
	}

	GLFramebuffer::GLFramebuffer(const FramebufferSpecification &specification)
		: mSpecification(specification)
	{
		auto count = specification.Attachments.GetColorAttachmentCount();
		const auto &specs = mSpecification.Attachments.GetColorAttachments();

		mColorAttachments.reserve(count);

		for (size_t i = 0; i < specs.size(); i++)
		{
			auto color_attachment_info = specs[i];
			color_attachment_info.CreateInfo.Aspect = ETextureAspect::Color;
			color_attachment_info.CreateInfo.Roles = ETextureRole::RenderTarget | ETextureRole::Sampled;
			color_attachment_info.CreateInfo.DebugName = std::format("{}_FB_Color{}", specification.DebugName, i);
			mColorAttachmentSpecifications.emplace_back(color_attachment_info);
		}

		auto depth = mSpecification.Attachments.GetDepthAttachment();
		if (IsDepthFormat(depth.CreateInfo.Format))
		{
			auto aspect = depth.CreateInfo.Format == EFormat::DEPTH24_STENCIL8 ? ETextureAspect::DepthStencil : ETextureAspect::Depth;

			mDepthSpecification = depth;
			mDepthSpecification.CreateInfo.Aspect = aspect;
			mDepthSpecification.CreateInfo.Roles = ETextureRole::DepthTarget | ETextureRole::Sampled;
			mDepthSpecification.CreateInfo.DebugName = specification.DebugName + "_FB_Depth";
		}

		mColorAttachments.reserve(mColorAttachmentSpecifications.size());

		Initialize();
	}

	GLFramebuffer::~GLFramebuffer()
	{
	}

	void GLFramebuffer::BeginRendering(const ImageSubresourceRange &range)
	{
		for (size_t i = 0; i < mColorAttachments.size(); ++i)
		{
			AttachFramebufferTexture(
				mRendererID,
				GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i),
				mColorAttachments[i]->GetTextureID(),
				mColorAttachmentSpecifications[i].Type,
				range.BaseMipLevel,
				range.BaseArrayLayer);
		}

		if (mDepthAttachment)
		{
			AttachFramebufferTexture(
				mRendererID,
				GL_DEPTH_ATTACHMENT,
				mDepthAttachment->GetTextureID(),
				mDepthSpecification.Type,
				range.BaseMipLevel,
				range.BaseArrayLayer);
		}

		ASSERT(glCheckNamedFramebufferStatus(mRendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		const glm::uvec2 mipSize{
			std::max(mSpecification.Size.x >> range.BaseMipLevel, 1u),
			std::max(mSpecification.Size.y >> range.BaseMipLevel, 1u)};
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glViewport(0, 0, mipSize.x, mipSize.y);
	}

	void GLFramebuffer::EndRendering(const ImageSubresourceRange &range)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer::Resize(const glm::uvec2 &newSize)
	{
		if (newSize.x <= 0 || newSize.y <= 0 || newSize.x > sMaxFramebufferSize || newSize.y > sMaxFramebufferSize)
		{
			LOG_WARN("Attempted to resize framebuffer to {}", newSize);
			return;
		}

		mSpecification.Size = newSize;

		Initialize();
	}

	void GLFramebuffer::ClearAttachment(uint32_t attachmentIndex, const int *data)
	{
		glClearNamedFramebufferiv(mRendererID, GL_COLOR, attachmentIndex, data);
	}

	void GLFramebuffer::ClearAttachment(uint32_t attachmentIndex, const float *data)
	{
		glClearNamedFramebufferfv(mRendererID, GL_COLOR, attachmentIndex, data);
	}

	void GLFramebuffer::Blit(const Ref<Framebuffer> &target)
	{
	}

	void GLFramebuffer::BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h)
	{
		if (mColorAttachments.empty() || w == 0 || h == 0)
			return;

		glNamedFramebufferReadBuffer(mRendererID, GL_COLOR_ATTACHMENT0);
		glBlitNamedFramebuffer(
			mRendererID,
			0,
			0,
			0,
			static_cast<GLint>(mSpecification.Size.x),
			static_cast<GLint>(mSpecification.Size.y),
			static_cast<GLint>(x),
			static_cast<GLint>(y),
			static_cast<GLint>(x + w),
			static_cast<GLint>(y + h),
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);
	}

	void GLFramebuffer::ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const
	{
	}

	Ref<Texture> GLFramebuffer::GetColorAttachment(uint32_t index) const
	{
		ASSERT(index >= 0 && index < mColorAttachments.size())

		return mColorAttachments.at(index);
	}

	Ref<Texture> GLFramebuffer::GetDepthAttachment() const
	{
		return mDepthAttachment;
	}

	void GLFramebuffer::Initialize()
	{
		if (mRendererID)
		{
			mColorAttachments.clear();
			mDepthAttachment = {};
			glDeleteFramebuffers(1, &mRendererID);
		}

		glCreateFramebuffers(1, &mRendererID);

		auto numColorAttachments = mColorAttachmentSpecifications.size();
		std::vector<GLenum> drawBuffers(numColorAttachments);

		if (numColorAttachments)
		{
			mColorAttachments.resize(numColorAttachments);
			for (size_t i = 0; i < numColorAttachments; i++)
			{
				auto &specs = mColorAttachmentSpecifications[i];
				mColorAttachments[i] = CreateTexture(mSpecification.Size, mSpecification.Samples, specs);
				AttachFramebufferTexture(mRendererID, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i), mColorAttachments[i]->GetTextureID(), specs.Type, 0, 0);
				drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}
		}

		if (mDepthSpecification.CreateInfo.Format != EFormat::None)
		{
			mDepthAttachment = CreateTexture(mSpecification.Size, mSpecification.Samples, mDepthSpecification);
			AttachFramebufferTexture(mRendererID, GL_DEPTH_ATTACHMENT, mDepthAttachment->GetTextureID(), mDepthSpecification.Type, 0, 0);
		}

		if (drawBuffers.size())
		{
			glNamedFramebufferDrawBuffers(mRendererID, drawBuffers.size(), drawBuffers.data());
		}
		else
		{
			glNamedFramebufferDrawBuffer(mRendererID, GL_NONE);
		}

		ASSERT(glCheckNamedFramebufferStatus(mRendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}
} // namespace BHive