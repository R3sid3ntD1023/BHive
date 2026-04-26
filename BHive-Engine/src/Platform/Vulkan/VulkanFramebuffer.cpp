#include "VulkanFramebuffer.h"
#include "gfx/Texture.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

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
		for (auto &spec : mSpecification.Attachments.GetAttachments())
		{
			if (IsDepthFormat(spec.CreateInfo.Format))
			{
				mDepthSpecification = spec;
				mDepthSpecification.CreateInfo.Aspect = ETextureAspect::DepthStencil;
				mDepthSpecification.CreateInfo.Usage = ETextureUsage::DepthAttachment | ETextureUsage::Sampled;
				continue;
			}

			auto color_attachment_info = spec;
			color_attachment_info.CreateInfo.Aspect = ETextureAspect::Color;
			color_attachment_info.CreateInfo.Usage = ETextureUsage::ColorAttachment | ETextureUsage::Sampled;
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

		RenderCommand::BeginFrame();
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
					auto tex = color_attachments[i]->GetNativeHandle().As<GPUImage>();

					ImageSubresource sub{
						.MipLevel = spec.MipLevel,
						.BaseArrayLayer = spec.Layer,
						.LayerCount = spec.LayerCount
					};
					tex->Transition(vk_ctx.CommandBuffer, ImageState::ColorAttachment(), sub);
				}

				if (depth_attachment)
				{
					auto &spec = depth_specification;
					auto tex = depth_attachment->GetNativeHandle().As<GPUImage>();

					ImageSubresource sub{
						.MipLevel = spec.MipLevel,
						.BaseArrayLayer = spec.Layer, .LayerCount = spec.LayerCount
					};
					tex->Transition(vk_ctx.CommandBuffer, ImageState::DepthStencilAttachmentment());
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

				vk::Viewport viewport(0.f, 0.f, (float)spec.Size.x, (float)spec.Size.y, 0.0f, 1.0f);
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

		auto &pass = RenderCommand::GetActivePass();

		pass.CommandList.Push("UnBind Framebuffer",
		[color_attachments](IRendererContext & ctx)
		{
			auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
			vk_ctx.CommandBuffer.endRendering();

			for (size_t i = 0; i < color_attachments.size(); i++)
			{
				auto tex = color_attachments[i]->GetNativeHandle().As<GPUImage>();
				tex->Transition(vk_ctx.CommandBuffer, ImageState::ShaderRead());
			}
		});

		RenderCommand::EndPass();
		auto& graph = RenderCommand::EndFrame();
		FResourceUpdateList list{};
		RenderCommand::SubmitGraph(graph, list);

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

		auto &pass = RenderCommand::GetActivePass();
		auto& colorAttachments = mColorAttachments;

		RenderCommand::SubmitResourceUpdate([colorAttachments](const IRendererContext &ctx)
		{
			auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

			for (auto &tex : colorAttachments)
			{
				auto vkTex = tex->GetNativeHandle().As<GPUImage>();
				vkTex->Transition(vk_ctx.CommandBuffer, ImageState::ColorAttachment());
				vkTex->Transition(vk_ctx.CommandBuffer, ImageState::ShaderRead());
			}
		});
	}

} // namespace BHive