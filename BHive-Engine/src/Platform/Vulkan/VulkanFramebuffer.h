#pragma once

#include "VulkanCore.h"
#include "gfx/Framebuffer.h"


namespace BHive
{

	class BHIVE_API VulkanFramebuffer :public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification &specification);

		~VulkanFramebuffer() = default;

		void BindFace(uint32_t face);

		void Resize(const glm::uvec2 &newSize);

		void ClearAttachment(uint32_t attachmentIndex, const int *data);

		void ClearAttachment(uint32_t attachmentIndex, const float *data);

		void Blit(const Ref<Framebuffer> &target);

		void BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h);

		void ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const;

		uint32_t GetNumColorAttachments() const override { return (uint32_t)mColorAttachments.size(); }

		Ref<Texture> GetColorAttachment(uint32_t index = 0) const;

		Ref<Texture> GetDepthAttachment() const;

		const FramebufferSpecification &GetSpecification() const { return mSpecification; }

		const glm::uvec2& GetSize() const { return mSpecification.Size; }

		const FFramebufferTexture& GetColorAttachmentSpecs(uint32_t index) const override
		{
			ASSERT(index < mColorAttachmentSpecifications.size());
			return mColorAttachmentSpecifications[index];
		}

		const FFramebufferTexture& GetDepthAttachmentSpecs() const override { return mDepthSpecification; }

		uint32_t GetCurrentFace() const { return mCurrentFace; }

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