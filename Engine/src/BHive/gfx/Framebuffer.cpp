#include "Framebuffer.h"
#include <glad/glad.h>
#include "TextureUtils.h"
#include "textures/Texture2D.h"
#include "textures/Texture2DArray.h"
#include "textures/TextureCube.h"
#include "textures/TextureCubeArray.h"

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	Framebuffer::Framebuffer(const FramebufferSpecification &specification)
		: mSpecification(specification)
	{
		for (auto &spec : mSpecification.Attachments.GetAttachments())
		{
			if (IsDepthFormat(spec.mSpecification.InternalFormat))
			{
				mDepthSpecification = spec;
				continue;
			}

			mColorSpecifications.push_back(spec);
		}

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

	void Framebuffer::ClearAttachment(uint32_t attachmentIndex, unsigned type, const float *data)
	{
		ASSERT(attachmentIndex < mColorAttachments.size());

		auto &attachment = mColorAttachments[attachmentIndex];
		auto &spec = mColorSpecifications[attachmentIndex];

		glClearNamedFramebufferfv(mFramebufferID, GL_COLOR, attachmentIndex, data);
	}

	void Framebuffer::Blit(Ref<Framebuffer> &target)
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

	void Framebuffer::ReadPixel(
		uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data) const
	{
		ASSERT(attachmentIndex < mColorSpecifications.size());

		auto &spec = mColorSpecifications[attachmentIndex];

		glNamedFramebufferReadBuffer(mFramebufferID, GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glReadPixels(x, y, w, h, GetGLFormat(spec.mSpecification.InternalFormat), type, data);
	}

	Ref<Texture> Framebuffer::GetColorAttachment(uint32_t index) const
	{
		ASSERT(index < mColorAttachments.size());
		return mColorAttachments[index];
	}

	Ref<Texture> Framebuffer::GetDepthAttachment() const
	{
		ASSERT(mDepthAttachment);
		return mDepthAttachment;
	}

	void Framebuffer::Release()
	{

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
					attachment =
						CreateRef<Texture2D>(mSpecification.Width, mSpecification.Height, specification.mSpecification);
					break;
				case ETextureType::TEXTURE_CUBE_MAP:
					attachment = CreateRef<TextureCube>(mSpecification.Width, specification.mSpecification);
					break;
				case ETextureType::TEXTURE_ARRAY_2D:
					attachment = CreateRef<Texture2DArray>(
						mSpecification.Width, mSpecification.Height, mSpecification.Depth, specification.mSpecification);
					break;
				case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
					attachment = CreateRef<TextureCubeArray>(
						mSpecification.Width, mSpecification.Height, mSpecification.Depth, specification.mSpecification);
					break;
				default:
					break;
				}
			}
		}

		if (mDepthSpecification.mSpecification.InternalFormat != EFormat::Invalid)
		{
			switch (mDepthSpecification.TextureType)
			{
			case ETextureType::TEXTURE_2D:
				mDepthAttachment =
					CreateRef<Texture2D>(mSpecification.Width, mSpecification.Height, mDepthSpecification.mSpecification);
				break;
			case ETextureType::TEXTURE_CUBE_MAP:
				mDepthAttachment = CreateRef<TextureCube>(mSpecification.Width, mDepthSpecification.mSpecification);
				break;
			case ETextureType::TEXTURE_ARRAY_2D:
				mDepthAttachment = CreateRef<Texture2DArray>(
					mSpecification.Width, mSpecification.Height, mSpecification.Depth, mDepthSpecification.mSpecification);
				break;
			case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
				mDepthAttachment = CreateRef<TextureCubeArray>(
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
				mFramebufferID, GetDepthAttachmentType(mDepthSpecification.mSpecification.InternalFormat), *mDepthAttachment,
				0);
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
				mRenderBufferID, GetGLInternalFormat(specification.mSpecification.InternalFormat), mSpecification.Width,
				mSpecification.Height);
			glNamedFramebufferRenderbuffer(
				mFramebufferID, GetDepthAttachmentType(specification.mSpecification.InternalFormat), GL_RENDERBUFFER,
				mRenderBufferID);
		}

		ASSERT(glCheckNamedFramebufferStatus(mFramebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

} // namespace BHive