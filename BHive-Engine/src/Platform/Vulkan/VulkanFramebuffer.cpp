#include "VulkanFramebuffer.h"
#include "gfx/Texture.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "textures/VulkanImage.h"
#include "VulkanBackend.h"

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	Ref<Texture> CreateFramebufferTexture(const glm::uvec2& size, uint32_t samples, const FFramebufferTexture& specification)
	{
		if (specification.ExistingTexture)
			return specification.ExistingTexture;

		auto &type = specification.TextureType;

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
		const auto& specs = mSpecification.Attachments.GetAttachments();
		for (size_t i = 0; i < specs.size(); i++)
		{
			auto &spec = specs[i];
			if (IsDepthFormat(spec.CreateInfo.Format))
			{
				mDepthSpecification = spec;
				mDepthSpecification.CreateInfo.Aspect = ETextureAspect::DepthStencil;
				mDepthSpecification.CreateInfo.Roles = ETextureRole::DepthTarget | ETextureRole::Sampled;
				mDepthSpecification.CreateInfo.DebugName = "Framebuffer_Depth";
				continue;
			}

			auto color_attachment_info = spec;
			color_attachment_info.CreateInfo.Aspect = ETextureAspect::Color;
			color_attachment_info.CreateInfo.Roles = ETextureRole::RenderTarget | ETextureRole::Sampled;
			color_attachment_info.CreateInfo.DebugName = std::format("Framebuffer_Color{}", i);
			mColorAttachmentSpecifications.emplace_back(color_attachment_info);
		}

		mColorAttachments.reserve(mColorAttachmentSpecifications.size());

		Initialize();
	}

	void VulkanFramebuffer::Bind() const
	{
		auto color_attachments = mColorAttachments;
		auto color_specifications = mColorAttachmentSpecifications;
		auto depth_attachment = mDepthAttachment;
		auto depth_specification = mDepthSpecification;
		auto spec = mSpecification;
		auto current_face = mCurrentFace;

		auto &pass = RenderCommand::BeginPass("Framebuffer", EPassType::OffScreen);
		pass.CommandList.Push(
			"Bind Framebuffer",
			[current_face, color_attachments, color_specifications, depth_attachment, depth_specification, spec](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				// transition images
				for (size_t i = 0; i < color_attachments.size(); i++)
				{
					auto &spec = color_specifications[i];
					auto tex = color_attachments[i]->GetNativeHandle().As<VulkanImage>();

					ImageSubresource sub{spec.MipLevel, 1,  spec.Layer, spec.LayerCount};
					tex->Transition(vk_ctx.CommandBuffer, ImageState::ColorAttachment(), sub);
				}

				if (depth_attachment)
				{
					auto &spec = depth_specification;
					auto tex = depth_attachment->GetNativeHandle().As<VulkanImage>();

					tex->Transition(vk_ctx.CommandBuffer, ImageState::DepthStencilAttachment());
				}

				// render
				std::vector<vk::RenderingAttachmentInfo> color_infos;
				vk::RenderingAttachmentInfo depth_info{};

				const auto num_color_attachments = color_attachments.size();
				color_infos.reserve(num_color_attachments);

				for (size_t i = 0; i < num_color_attachments; i++)
				{
					auto& spec = color_specifications[i];
					auto vkTex = reinterpret_cast<VkImageView>(color_attachments[i]->GetRenderView(current_face, spec.MipLevel).AsRaw());

					auto info = vk::RenderingAttachmentInfo(
						vkTex, vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
						vk::ClearColorValue(0, 0, 0, 1));

					color_infos.emplace_back(info);
				}

				if (depth_attachment)
				{
					auto &spec = depth_specification;
					auto vkTex = reinterpret_cast<VkImageView>(depth_attachment->GetRenderView(current_face, spec.MipLevel).AsRaw());

					depth_info = vk::RenderingAttachmentInfo(
						vkTex, vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
						vk::ClearDepthStencilValue(1.0f, 0));
				}

				auto rect = vk::Rect2D({0, 0}, {spec.Size.x, spec.Size.y});
				auto depth = depth_attachment ? &depth_info : nullptr;
				auto info = vk::RenderingInfo({}, rect, 1, 0, color_infos, depth);

				vk_ctx.CommandBuffer.beginRendering(info);

				vk::Viewport viewport(0.f, (float)spec.Size.y, (float)spec.Size.x, -(float)spec.Size.y, 0.0f, 1.0f);
				vk::Rect2D scissor({0, 0}, {spec.Size.x, spec.Size.y});

				vk_ctx.CommandBuffer.setViewport(0, viewport);
				vk_ctx.CommandBuffer.setScissor(0, scissor);
			});
	}

	void VulkanFramebuffer::BindFace(uint32_t face)
	{
		mCurrentFace = face;
	}

	void VulkanFramebuffer::UnBind() const
	{
		auto color_attachments = mColorAttachments;

		RenderCommand::SubmitCommand("UnBind Framebuffer",
		[color_attachments](IRendererContext & ctx)
		{
			auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
			vk_ctx.CommandBuffer.endRendering();

			for (size_t i = 0; i < color_attachments.size(); i++)
			{
				auto tex = color_attachments[i]->GetNativeHandle().As<VulkanImage>();
				tex->Transition(vk_ctx.CommandBuffer, ImageState::ShaderRead());
			}
		});

		RenderCommand::EndPass();
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
		ASSERT(attachmentIndex < mColorAttachments.size());

		
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

		auto& colorAttachments = mColorAttachments;

		RenderCommand::GetGraphicsAPI()->ExecuteTransferPass([=](ITransferContext& ctx) {
				auto &transfer_ctx = CastRef<FVulkanTransferContext>(ctx);
				for (auto &tex : colorAttachments)
				{
					auto vkTex = tex->GetNativeHandle().As<VulkanImage>();
					vkTex->Transition(transfer_ctx.Cmd, ImageState::ColorAttachment());
					vkTex->Transition(transfer_ctx.Cmd, ImageState::ShaderRead());
				}
			});
	}

} // namespace BHive