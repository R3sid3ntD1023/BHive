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

		virtual void BindFace(uint32_t face);

		virtual void UnBind() const;

		virtual void Resize(const glm::uvec2 &newSize);

		virtual void ClearAttachment(uint32_t attachmentIndex, const int *data);

		virtual void ClearAttachment(uint32_t attachmentIndex, const float *data);

		virtual void Blit(const Ref<Framebuffer> &target);

		virtual void BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h);

		virtual void ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const;

		virtual uint32_t GetNumColorAttachments() const override { return (uint32_t)mColorAttachments.size(); }

		virtual Ref<Texture> GetColorAttachment(uint32_t index = 0) const;

		virtual Ref<Texture> GetDepthAttachment() const;

		virtual const FramebufferSpecification &GetSpecification() const { return mSpecification; }

		virtual const glm::uvec2& GetSize() const { return mSpecification.Size; }

	private:
		void Initialize();

	private:
		std::vector<FFramebufferTexture> mColorAttachmentSpecifications{};

		FFramebufferTexture mDepthSpecification{};

		std::vector<Ref<Texture>> mColorAttachments;
		Ref<Texture> mDepthAttachment;

		FramebufferSpecification mSpecification;

		uint32_t mCurrentFace = 0;
	};

} // namespace BHive