#pragma once

#include "gfx/Framebuffer.h"
#include "gfx/resources/ImageSubResourceRange.h"

namespace BHive
{
	class GLFramebuffer : public Framebuffer
	{
	public:
		GLFramebuffer(const FramebufferSpecification &specification);

		~GLFramebuffer();

		void BeginRendering(const ImageSubresourceRange &range);

		void EndRendering(const ImageSubresourceRange &range);

		void Resize(const glm::uvec2 &newSize) override;

		void ClearAttachment(uint32_t attachmentIndex, const int *data) override;

		void ClearAttachment(uint32_t attachmentIndex, const float *data) override;

		void Blit(const Ref<Framebuffer> &target) override;

		void BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h) override;

		void ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const override;

		uint32_t GetNumColorAttachments() const override { return (uint32_t)mColorAttachments.size(); }

		Ref<Texture> GetColorAttachment(uint32_t index = 0) const override;

		Ref<Texture> GetDepthAttachment() const override;

		const FramebufferSpecification &GetSpecification() const override { return mSpecification; }

		const glm::uvec2 &GetSize() const override { return mSpecification.Size; }

		const FFramebufferTexture &GetColorAttachmentSpecs(uint32_t index) const override
		{
			ASSERT(index < mColorAttachmentSpecifications.size());
			return mColorAttachmentSpecifications[index];
		}

		const FFramebufferTexture &GetDepthAttachmentSpecs() const override { return mDepthSpecification; }

	private:
		void Initialize();

	private:
		uint32_t mRendererID = 0;

		FramebufferSpecification mSpecification;

		std::vector<FFramebufferTexture> mColorAttachmentSpecifications{};

		FFramebufferTexture mDepthSpecification{};

		std::vector<Ref<Texture>> mColorAttachments;

		Ref<Texture> mDepthAttachment;
	};
} // namespace BHive