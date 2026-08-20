#pragma once

#include "VulkanCore.h"
#include "gfx/Framebuffer.h"

namespace BHive
{

	class BHIVE_API VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification &specification);

		~VulkanFramebuffer() = default;

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

		struct RenderInfo
		{
			vk::ClearColorValue ClearColor;
			vk::ClearDepthStencilValue ClearDepthValue;
			vk::AttachmentLoadOp ColorLoadOp;
			vk::AttachmentStoreOp ColorStoreOp;
			vk::AttachmentLoadOp DepthLoadOp;
			vk::AttachmentStoreOp DepthStoreOp;
			vk::ImageSubresourceRange ColorRange;
		};

		void BeginRendering(vk::CommandBuffer cmd, const RenderInfo &info);

		void EndRendering(vk::CommandBuffer cmd);

	private:
		void Initialize();

	private:
		std::vector<FFramebufferTexture> mColorAttachmentSpecifications{};

		FFramebufferTexture mDepthSpecification{};

		std::vector<Ref<Texture>> mColorAttachments;
		Ref<Texture> mDepthAttachment;

		FramebufferSpecification mSpecification;
	};

} // namespace BHive