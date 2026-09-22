#pragma once

#include "TextureSpecification.h"
#include "core/Core.h"
#include "core/math/Math.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	struct FFramebufferTexture
	{
		FTextureCreateInfo CreateInfo{};
		ETextureType Type = ETextureType::TEXTURE_2D;
		TexturePtr ExternalTexture = {};

		FFramebufferTexture() = default;
		FFramebufferTexture(const FTextureCreateInfo &createInfo, ETextureType type = ETextureType::TEXTURE_2D)
			: CreateInfo(createInfo),
			  Type(type)
		{
		}

		FFramebufferTexture(TexturePtr texture, ETextureType type = ETextureType::TEXTURE_2D)
			: Type(type),
			  ExternalTexture(texture)
		{
		}
	};

	struct BHIVE_API FramebufferAttachments
	{
		FramebufferAttachments() = default;

		FramebufferAttachments &Reset()
		{
			mColorAttachments.clear();
			mDepthAttachment = {};
			return *this;
		}

		FramebufferAttachments &AddColorAttachment(const FFramebufferTexture &spec)
		{
			mColorAttachments.emplace_back(spec);
			return *this;
		}

		FramebufferAttachments &AddColorAttachment(const FTextureCreateInfo &createInfo, ETextureType type = ETextureType::TEXTURE_2D)
		{
			return AddColorAttachment(FFramebufferTexture{createInfo, type});
		}

		FramebufferAttachments &SetDepthAttachment(const FFramebufferTexture &spec)
		{
			mDepthAttachment = spec;
			return *this;
		}

		FramebufferAttachments &SetDepthAttachment(const FTextureCreateInfo &createInfo, ETextureType type = ETextureType::TEXTURE_2D)
		{
			return SetDepthAttachment(FFramebufferTexture{createInfo, type});
		}

		const std::vector<FFramebufferTexture> &GetColorAttachments() const { return mColorAttachments; }

		const FFramebufferTexture &GetDepthAttachment() const { return mDepthAttachment; }

		uint32_t GetColorAttachmentCount() const { return mColorAttachments.size(); }

	private:
		std::vector<FFramebufferTexture> mColorAttachments;
		FFramebufferTexture mDepthAttachment;

		friend class Framebuffer;
	};

	struct BHIVE_API FramebufferSpecification
	{
		FramebufferAttachments Attachments;
		glm::uvec2 Size{800, 600};
		uint32_t Samples = 1;
		std::string DebugName = "";
	};

	class BHIVE_API Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Resize(const glm::uvec2 &newSize) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, const int *data) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, const float *data) = 0;

		virtual void Blit(const Ref<Framebuffer> &target) = 0;

		virtual void BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h) = 0;

		virtual void ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const = 0;

		virtual uint32_t GetNumColorAttachments() const = 0;

		virtual TexturePtr GetColorAttachment(uint32_t index = 0) const = 0;

		virtual TexturePtr GetDepthAttachment() const = 0;

		virtual const FramebufferSpecification &GetSpecification() const = 0;

		virtual const glm::uvec2 &GetSize() const = 0;

		virtual const FFramebufferTexture &GetColorAttachmentSpecs(uint32_t i) const = 0;

		virtual const FFramebufferTexture &GetDepthAttachmentSpecs() const = 0;
	};

} // namespace BHive