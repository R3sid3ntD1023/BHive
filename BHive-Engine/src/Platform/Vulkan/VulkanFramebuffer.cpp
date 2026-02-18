#include "VulkanFramebuffer.h"
#include "gfx/Texture.h"
#include "textures/VulkanTexture2D.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	static const uint32_t sMaxFramebufferSize = 8192;

	Ref<Texture> CreateFramebufferTexture(uint32_t w, uint32_t h, uint32_t d, uint32_t samples, FFramebufferTexture specification)
	{
		auto &type = specification.TextureType;

		if (samples > 1 && type == ETextureType::TEXTURE_2D)
			return Texture2DMultisample::Create(w, h, samples, specification.CreateInfo);

		switch (type)
		{
		case ETextureType::TEXTURE_2D:
			return Texture2D::Create(w, h, specification.CreateInfo);
		case ETextureType::TEXTURE_CUBE_MAP:
			return TextureCube::Create(w, specification.CreateInfo);
		case ETextureType::TEXTURE_2D_ARRAY:
			return Texture2DArray::Create(w, h, d, specification.CreateInfo);
		case ETextureType::TEXTURE_CUBE_MAP_ARRAY:
			return TextureCubeArray::Create(w, h, d, specification.CreateInfo);
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
				continue;
			}

			mColorAttachmentSpecifications.emplace_back(spec);
		}

		mRenderBufferSpecification = mSpecification.Attachments.GetRenderBuffer();

		Initialize();
	}

	void VulkanFramebuffer::Bind() const
	{
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto cmd = [this, api](const FVulkanFrameData &frame) 
		{
			api->BeginFramebuffer(this);

			frame.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)(y + h), (float)w, -(float)h, 0.0f, 1.0f));
			frame.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, vk::Extent2D(w, h)));
		};

		api->SubmitCommand(cmd);
	}

	void VulkanFramebuffer::UnBind() const
	{

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [api](const FVulkanFrameData &frame) { api->EndFramebuffer(); };

		api->SubmitCommand(cmd);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > sMaxFramebufferSize || height > sMaxFramebufferSize)
		{
			LOG_WARN("Attempted to resize framebuffer to <{},{}>", width, height);
			return;
		}

		mSpecification.Width = width;
		mSpecification.Height = height;

		Initialize();
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, const int *data)
	{
		ASSERT(attachmentIndex < mColorAttachments.size());

		//glClearNamedFramebufferiv(mFramebufferID, GL_COLOR, attachmentIndex, data);
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, const float *data)
	{
		//glClearNamedFramebufferfv(mFramebufferID, GL_COLOR, attachmentIndex, data);
	}

	void VulkanFramebuffer::Blit(const Ref<Framebuffer> &target)
	{
		if (!target || mSpecification.Width == 0 || mSpecification.Height == 0)
			return;

		const auto &specs = mSpecification;

		if (!target || specs.Width == 0 || specs.Height == 0)
			return;

		const auto &dst_specs = target->GetSpecification();
		auto count = dst_specs.Attachments.GetAttachments().size();

		/*const auto read_target = mFramebufferID;
		const auto draw_target = target ? target->GetRendererID() : 0;*/

		for (size_t i = 0; i < count; i++)
		{
			/*glNamedFramebufferReadBuffer(read_target, GL_COLOR_ATTACHMENT0 + i);
			glNamedFramebufferDrawBuffer(draw_target, GL_COLOR_ATTACHMENT0 + i);

			glBlitNamedFramebuffer(read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);*/
		}

		if (target->GetDepthAttachment() && GetDepthAttachment())
		{
			//glBlitNamedFramebuffer(read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, dst_specs.Width, dst_specs.Height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		}
	}

	void VulkanFramebuffer::BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h)
	{
		const auto &specs = mSpecification;

		if (specs.Width == 0 || specs.Height == 0)
			return;

		//glNamedFramebufferReadBuffer(read_target, GL_COLOR_ATTACHMENT0);

		//glBlitNamedFramebuffer(read_target, draw_target, 0, 0, specs.Width, specs.Height, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const
	{
		ASSERT(attachmentIndex < mColorAttachmentSpecifications.size());

		//auto &spec = mColorAttachmentAPIInfos[attachmentIndex];

	/*	glNamedFramebufferReadBuffer(mFramebufferID, GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glReadPixels(x, y, w, h, spec.Format, spec.Type, data);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
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


	vk::RenderingInfo VulkanFramebuffer::BuildRenderingInfo() const
	{
		std::vector<vk::RenderingAttachmentInfo> color_infos;
		color_infos.reserve(mColorAttachments.size());

		for (auto& tex : mColorAttachments)
		{
			auto vkTex = Cast<IVulkanTexture>(tex);

			auto info = vk::RenderingAttachmentInfo(
				vkTex->GetImageView(), vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::ClearDepthStencilValue(1.0f, 0));

			color_infos.push_back(info);
		}

		vk::RenderingAttachmentInfo depth_info{};

		if (mDepthAttachment)
		{
			auto vkTex = Cast<IVulkanTexture>(mDepthAttachment);

			depth_info =  vk::RenderingAttachmentInfo(
				vkTex->GetImageView(), vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear,
				vk::AttachmentStoreOp::eStore, vk::ClearDepthStencilValue(1.0f, 0));
		}

		return vk::RenderingInfo({}, vk::Rect2D{0, 0}, mSpecification.Width, mSpecification.Height, color_infos, mDepthAttachment ? &depth_info : nullptr);
	}

	void VulkanFramebuffer::Initialize()
	{
		auto numColorAttachments = mColorAttachmentSpecifications.size();
		if (numColorAttachments)
		{
			mColorAttachments.resize(numColorAttachments);
			for (size_t i = 0; i < numColorAttachments; i++)
			{
				auto &specs = mColorAttachmentSpecifications[i];
				auto &attachment = mColorAttachments[i];

				attachment = CreateFramebufferTexture(mSpecification.Width, mSpecification.Height, mSpecification.Depth, mSpecification.Samples, specs);
			}
		}

		if (mDepthSpecification.CreateInfo.Format != EFormat::None)
		{
			mDepthAttachment = CreateFramebufferTexture(mSpecification.Width, mSpecification.Height, mSpecification.Depth, mSpecification.Samples, mDepthSpecification);
		}

		
	}

} // namespace BHive