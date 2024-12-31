#pragma once

#include "gfx/Framebuffer.h"

namespace BHive
{
	class GLFramebuffer : public Framebuffer
	{
	public:
		GLFramebuffer() = default;
		GLFramebuffer(const FramebufferSpecification &specification);
		virtual ~GLFramebuffer();

		void Bind() const;
		void UnBind() const;

		void Resize(uint32_t width, uint32_t height);
		void ClearAttachment(uint32_t attachmentIndex, unsigned type, const void *data);
		void Blit(Framebuffer *target);

		void ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data);

		void BindForRead();

		Ref<Texture> GetColorAttachment(uint32_t index = 0) const { return mColorAttachments[index]; }
		Ref<Texture> GetDepthAttachment() const { return mDepthAttachment; }

		const FramebufferSpecification &GetSpecification() const { return mSpecification; }
		uint32_t GetWidth() const { return mSpecification.Width; }
		uint32_t GetHeight() const { return mSpecification.Height; }

		uint32_t GetRendererID() const { return mFramebufferID; }
		operator uint32_t() const { return GetRendererID(); }

		bool SaveToImage(const std::filesystem::path &path);
		static bool SaveToImage(Framebuffer &framebuffer, const std::filesystem::path &path);

	protected:
		virtual void Initialize();
		virtual void Release();

		void BlitInternal(Framebuffer *target);
		void ReadPixelInternal(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data);

	protected:
		std::vector<FFramebufferTexture> mColorSpecifications;
		FFramebufferTexture mDepthSpecification;

		std::vector<Ref<Texture>> mColorAttachments;
		Ref<Texture> mDepthAttachment;

		FramebufferSpecification mSpecification;

		uint32_t mFramebufferID{0};
		uint32_t mRenderBufferID{0};
	};
}