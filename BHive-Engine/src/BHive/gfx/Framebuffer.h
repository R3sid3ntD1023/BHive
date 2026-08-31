#pragma once

#include "core/Core.h"
#include "core/math/Math.h"
#include "gfx/registries/Handles.h"
#include "TextureSpecification.h"

namespace BHive
{
	struct FFramebufferTexture
	{
		FTextureCreateInfo CreateInfo{};
		ETextureType Type = ETextureType::TEXTURE_2D;
		TexturePtr ExternalTexture = {};
	};

	struct FRenderbufferTexture
	{
		EFormat Format = EFormat::None;
	};

	struct BHIVE_API FramebufferAttachments
	{
		FramebufferAttachments() = default;

		FramebufferAttachments &Reset()
		{
			mColorAttachments.clear();
			mDepthAttachment = {};
			mRenderBufferSpecification = {};
			return *this;
		}

		FramebufferAttachments &AddColorAttachment(const FFramebufferTexture &spec)
		{
			mColorAttachments.emplace_back(spec);
			return *this;
		}

		FramebufferAttachments &SetDepthAttachment(const FFramebufferTexture &spec)
		{
			mDepthAttachment = spec;
			return *this;
		}

		FramebufferAttachments &SetRenderBuffer(const FRenderbufferTexture &format)
		{
			mRenderBufferSpecification = format;
			return *this;
		}

		const std::vector<FFramebufferTexture> &GetColorAttachments() const { return mColorAttachments; }

		const FFramebufferTexture &GetDepthAttachment() const { return mDepthAttachment; }

		const FRenderbufferTexture &GetRenderBuffer() const { return mRenderBufferSpecification; }

	private:
		std::vector<FFramebufferTexture> mColorAttachments;
		FFramebufferTexture mDepthAttachment;
		FRenderbufferTexture mRenderBufferSpecification;

		friend class Framebuffer;
	};

	struct BHIVE_API FramebufferSpecification
	{
		FramebufferAttachments Attachments;
		glm::uvec2 Size{800, 600};
		uint32_t Depth = 1;
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