#include "GLFramebuffer.h"
#include "gfx/TextureUtils.h"
#include <stb_image_write.h>
#include <glad/glad.h>
#include "gfx/Texture.h"
#include "threading/Threading.h"

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	GLFramebuffer::GLFramebuffer(const FramebufferSpecification &specification)
		: mSpecification(specification)
	{
		for (auto &spec : mSpecification.Attachments.GetAttachments())
		{
			if (IsDepthFormat(spec.mSpecification.Format))
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

	void GLFramebuffer::ClearAttachment(uint32_t attachmentIndex, unsigned type, const void *data)
	{
		ASSERT(attachmentIndex < mColorAttachments.size());

		auto &attachment = mColorAttachments[attachmentIndex];
		auto &spec = mColorSpecifications[attachmentIndex];

		glClearTexImage(attachment->GetRendererID(), 0, GetGLFormat(spec.mSpecification.Format), type, data);
		;
	}

	void GLFramebuffer::Blit(Framebuffer *target)
	{
		if (!target || mSpecification.Width == 0 || mSpecification.Height == 0)
			return;

		BlitInternal(target);
		;
	}

	void GLFramebuffer::ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data)
	{
		ASSERT(attachmentIndex < mColorSpecifications.size());

		ReadPixelInternal(attachmentIndex, x, y, w, h, type, data);
	}

	void GLFramebuffer::BindForRead()
	{

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferID);
	}

	bool GLFramebuffer::SaveToImage(const std::filesystem::path &path)
	{
		return SaveToImage(*this, path);
	}

	bool GLFramebuffer::SaveToImage(Framebuffer &framebuffer, const std::filesystem::path &path)
	{
		if (!std::filesystem::exists(path.parent_path()))
		{
			std::filesystem::create_directory(path.parent_path());
		}

		try
		{
			auto w = framebuffer.GetSpecification().Width;
			auto h = framebuffer.GetSpecification().Height;

			GLsizei channels = 4;
			GLsizei stride = channels * w;
			GLsizei buffersize = stride * h;
			std::vector<uint8_t> buffer(buffersize);

			framebuffer.ReadPixel(0, 0, 0, w, h, GL_UNSIGNED_BYTE, buffer.data());

			stbi_flip_vertically_on_write(true);
			stbi_write_png(path.string().c_str(), w, h, channels, buffer.data(), stride);

			return true;
		}
		catch (std::exception &e)
		{
			LOG_ERROR("Save Image {}", e.what());
		}

		return false;
	}

	void GLFramebuffer::Release()
	{

		glDeleteFramebuffers(1, &mFramebufferID);
	}

	void GLFramebuffer::BlitInternal(Framebuffer *target)
	{

		const auto &specs = mSpecification;

		if (!target || specs.Width == 0 || specs.Height == 0)
			return;

		const auto &dst_specs = target->GetSpecification();
		auto count = dst_specs.Attachments.GetAttachments().size();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->GetRendererID());

		for (size_t i = 0; i < count; i++)
		{
			glReadBuffer(GL_COLOR_ATTACHMENT0 + (uint32_t)i);
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + (uint32_t)i);
			glBlitFramebuffer(0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height, GL_COLOR_BUFFER_BIT,
							  GL_NEAREST);
		}

		if (target->GetDepthAttachment() && GetDepthAttachment())
		{
			glBlitFramebuffer(0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
							  GL_NEAREST);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer::ReadPixelInternal(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data)
	{
		auto &spec = mColorSpecifications[attachmentIndex];

		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glReadPixels(x, y, w, h, GetGLFormat(spec.mSpecification.Format), type, data);

		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
					attachment = Texture2D::Create(mSpecification.Width, mSpecification.Height, specification.mSpecification, mSpecification.Samples);
					break;
				case ETextureType::TEXTURE_CUBE_MAP:
					attachment = TextureCube::Create(mSpecification.Width, specification.mSpecification);
					break;
				case ETextureType::TEXTURE_3D:
					attachment = Texture3D::Create(mSpecification.Width, mSpecification.Height, mSpecification.Depth, specification.mSpecification);
				default:
					break;
				}
			}
		}

		if (mDepthSpecification.mSpecification.Format != EFormat::Invalid)
		{
			switch (mDepthSpecification.TextureType)
			{
			case ETextureType::TEXTURE_2D:
				mDepthAttachment = Texture2D::Create(mSpecification.Width, mSpecification.Height, mDepthSpecification.mSpecification, mSpecification.Samples);
				break;
			case ETextureType::TEXTURE_CUBE_MAP:
				mDepthAttachment = TextureCube::Create(mSpecification.Width, mDepthSpecification.mSpecification);
				break;
			case ETextureType::TEXTURE_3D:
				mDepthAttachment = Texture3D::Create(mSpecification.Width, mSpecification.Height, mSpecification.Depth, mDepthSpecification.mSpecification);
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
			glNamedFramebufferTexture(mFramebufferID, GetDepthAttachmentType(mDepthSpecification.mSpecification.Format), *mDepthAttachment, 0);
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

		if (mSpecification.mWriteOnly)
		{
			auto &specification = mSpecification.mRenderSpecification;
			glCreateRenderbuffers(1, &mRenderBufferID);
			glNamedRenderbufferStorage(mRenderBufferID, GetGLInternalFormat(specification.mSpecification.Format), mSpecification.Width, mSpecification.Height);
			glNamedFramebufferRenderbuffer(mFramebufferID, GetDepthAttachmentType(specification.mSpecification.Format), GL_RENDERBUFFER, mRenderBufferID);
		}

		ASSERT(glCheckNamedFramebufferStatus(mFramebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

}