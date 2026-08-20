#include "VulkanFramebuffer.h"
#include "VulkanRendererAPI.h"
#include "VulkanImage.h"
#include "VulkanBackend.h"
#include "gfx/Texture.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	Ref<Texture> CreateFramebufferTexture(const glm::uvec2 &size, uint32_t samples, const FFramebufferTexture &specification)
	{
		if (specification.ExternalTexture)
			return specification.ExternalTexture;

		auto &type = specification.Type;

		switch (type)
		{
		case ETextureType::TEXTURE_2D:
			return Texture2D::Create({size.x, size.y}, specification.CreateInfo);
		case ETextureType::TEXTURE_CUBE_MAP:
			return TextureCube::Create(size.x, specification.CreateInfo);
		case ETextureType::TEXTURE_2D_ARRAY:
			return Texture2DArray::Create({size.x, size.y}, specification.CreateInfo);
		case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
			return TextureCubeArray::Create(size.x, specification.CreateInfo);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification &specification)
		: mSpecification(specification)
	{
		const auto &specs = mSpecification.Attachments.GetColorAttachments();
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
			mDepthSpecification = depth;
			mDepthSpecification.CreateInfo.Aspect = ETextureAspect::DepthStencil;
			mDepthSpecification.CreateInfo.Roles = ETextureRole::DepthTarget | ETextureRole::Sampled;
			mDepthSpecification.CreateInfo.DebugName = specification.DebugName + "_FB_Depth";
		}

		mColorAttachments.reserve(mColorAttachmentSpecifications.size());

		Initialize();
	}

	void VulkanFramebuffer::Resize(const glm::uvec2 &newSize)
	{
		if (newSize.x <= 0 || newSize.y <= 0 || newSize.x > sMaxFramebufferSize || newSize.y > sMaxFramebufferSize)
		{
			LOG_WARN("Attempted to resize framebuffer to {}", newSize);
			return;
		}

		VulkanBackend::GetLogicalDevice().waitIdle();

		mSpecification.Size = newSize;

		LOG_TRACE("recreating fbo... with size[{}x{}]", newSize.x, newSize.y);

		Initialize();
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, const int *data)
	{
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, const float *data)
	{
	}

	void VulkanFramebuffer::Blit(const Ref<Framebuffer> &target)
	{
	}

	void VulkanFramebuffer::BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h)
	{
	}

	void VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const
	{
		ASSERT(attachmentIndex < mColorAttachmentSpecifications.size());
	}

	Ref<Texture> VulkanFramebuffer::GetColorAttachment(uint32_t index) const
	{
		ASSERT(index < mColorAttachments.size());
		return mColorAttachments[index];
	}

	Ref<Texture> VulkanFramebuffer::GetDepthAttachment() const
	{
		return mDepthAttachment;
	}

	void VulkanFramebuffer::Initialize()
	{
		mColorAttachments.clear();
		mDepthAttachment.reset();

		auto numColorAttachments = mColorAttachmentSpecifications.size();
		if (numColorAttachments)
		{
			mColorAttachments.resize(numColorAttachments);
			for (size_t i = 0; i < numColorAttachments; i++)
			{
				auto &specs = mColorAttachmentSpecifications[i];
				mColorAttachments[i] = CreateFramebufferTexture(mSpecification.Size, mSpecification.Samples, specs);
			}
		}

		if (mDepthSpecification.CreateInfo.Format != EFormat::None)
		{
			mDepthAttachment = CreateFramebufferTexture(mSpecification.Size, mSpecification.Samples, mDepthSpecification);
		}
	}

	void VulkanFramebuffer::BeginRendering(vk::CommandBuffer cmd, const VulkanFramebuffer::RenderInfo &info)
	{
		auto range = info.ColorRange;

		std::vector<vk::RenderingAttachmentInfo> color_infos;
		for (size_t i = 0; i < mColorAttachments.size(); i++)
		{
			auto attachment = mColorAttachments[i];
			auto &spec = mColorAttachmentSpecifications[i];
			auto view = Cast<IVulkanTextureInterface>(attachment)->ResolveRenderView(range.baseArrayLayer, range.baseMipLevel);

			auto colorInfo
				= vk::RenderingAttachmentInfo(view, vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eColorAttachmentOptimal, info.ColorLoadOp, info.ColorStoreOp, info.ClearColor);

			color_infos.emplace_back(colorInfo);
		}

		vk::RenderingAttachmentInfo depthInfo{};
		vk::RenderingAttachmentInfo *depthPtr = nullptr;

		if (mDepthAttachment)
		{
			auto &spec = mDepthSpecification;
			auto view = Cast<IVulkanTextureInterface>(mDepthAttachment)->ResolveRenderView(0, 0);

			depthInfo = vk::RenderingAttachmentInfo(
				view, vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eDepthStencilAttachmentOptimal, info.DepthLoadOp, info.DepthStoreOp, info.ClearDepthValue);

			depthPtr = &depthInfo;
		}

		glm::uvec2 baseSize = mSpecification.Size;
		glm::uvec2 mipSize = {std::max(baseSize.x >> range.baseMipLevel, 1u), std::max(baseSize.y >> range.baseMipLevel, 1u)};

		auto rect = vk::Rect2D({0, 0}, {mipSize.x, mipSize.y});
		auto renderInfo = vk::RenderingInfo({}, rect, 1, 0, color_infos, depthPtr);

		cmd.beginRendering(renderInfo);

		vk::Viewport viewport(0.f, (float)mipSize.y, (float)mipSize.x, -(float)mipSize.y, 0.0f, 1.0f);
		vk::Rect2D scissor({0, 0}, {mipSize.x, mipSize.y});

		cmd.setViewportWithCount(viewport);
		cmd.setScissorWithCount(scissor);
	}

	void VulkanFramebuffer::EndRendering(vk::CommandBuffer cmd)
	{
		cmd.endRendering();
	}

} // namespace BHive