#include "VulkanFramebuffer.h"
#include "VulkanRendererAPI.h"
#include "VulkanImage.h"
#include "VulkanBackend.h"
#include "gfx/Texture.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	Ref<Texture> CreateFramebufferTexture(const glm::uvec2& size, uint32_t samples, const FFramebufferTexture& specification)
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
		const auto& specs = mSpecification.Attachments.GetColorAttachments();
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

	
	void VulkanFramebuffer::BindFace(uint32_t face)
	{
		mCurrentFace = face;
	}

	void VulkanFramebuffer::Resize(const glm::uvec2 &newSize)
	{
		if (newSize.x == 0 || newSize.y == 0 ||  newSize.x > sMaxFramebufferSize || newSize.y > sMaxFramebufferSize)
		{
			LOG_WARN("Attempted to resize framebuffer to {}", newSize);
			return;
		}

		VulkanBackend::GetLogicalDevice().waitIdle();

		mSpecification.Size = newSize;

		Initialize();
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, const int *data)
	{
		/*ASSERT(attachmentIndex < mColorAttachments.size());

		auto cAttachment = mColorAttachments[attachmentIndex];
		auto cSpec = mColorAttachmentSpecifications[attachmentIndex];
		auto fbSpec = mSpecification;
		auto color = vk::ClearColorValue(data[0], data[1], data[2], data[3]);

		RenderCommand::SubmitCommand(
			"Clear Attachment",
			[cAttachment, cSpec, fbSpec, color](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				auto &cmd = vk_ctx.CommandBuffer;
				auto vimage = cAttachment->GetNativeHandle().As<VulkanImage>();
				auto layout = vimage->GetState(cSpec.MipLevel, cSpec.Layer).Layout;
				auto image = vimage->Native().GetImage();
			
				vk::ImageSubresourceRange range;
				range.aspectMask = vk::ImageAspectFlagBits::eColor;
				range.baseMipLevel = cSpec.MipLevel;
				range.baseArrayLayer = cSpec.Layer;
				range.layerCount = cSpec.CreateInfo.ArrayLayers;
				range.levelCount = 1;
				cmd.clearColorImage(image, layout, color , range);
			});
		*/
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
			mDepthAttachment = CreateFramebufferTexture(mSpecification.Size,  mSpecification.Samples, mDepthSpecification);
		}
	}

} // namespace BHive