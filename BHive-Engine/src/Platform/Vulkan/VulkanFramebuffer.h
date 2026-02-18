#pragma once

#include "core/Core.h"
#include "gfx/Texture.h"
#include "gfx/Framebuffer.h"
#include "VulkanBackend.h"

namespace BHive
{

	class BHIVE_API VulkanFramebuffer :public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification &specification);

		virtual ~VulkanFramebuffer() = default;

		virtual void Bind() const;

		virtual void UnBind() const;

		virtual void Resize(uint32_t width, uint32_t height);

		virtual void ClearAttachment(uint32_t attachmentIndex, const int *data);

		virtual void ClearAttachment(uint32_t attachmentIndex, const float *data);

		virtual void Blit(const Ref<Framebuffer> &target);

		virtual void BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h);

		virtual void ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const;

		virtual Ref<Texture> GetColorAttachment(uint32_t index = 0) const;

		virtual Ref<Texture> GetDepthAttachment() const;

		virtual const FramebufferSpecification &GetSpecification() const { return mSpecification; }

		virtual uint32_t GetWidth() const { return mSpecification.Width; }

		virtual uint32_t GetHeight() const { return mSpecification.Height; }

		vk::RenderingInfo BuildRenderingInfo() const;

	private:
		void Initialize();

	private:
		std::vector<FFramebufferTexture> mColorAttachmentSpecifications{};

		FFramebufferTexture mDepthSpecification{};

		FRenderbufferTexture mRenderBufferSpecification{};

		std::vector<Ref<Texture>> mColorAttachments;
		Ref<Texture> mDepthAttachment;

		FramebufferSpecification mSpecification;

	};

} // namespace BHive