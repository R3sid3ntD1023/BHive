#pragma once

#include "core/Core.h"
#include "core/math/Math.h"
#include "Texture.h"

namespace BHive
{
	class FramebufferTexture2D;
	class TextureCubeMap;

	struct FFramebufferTexture
	{
		FTextureCreateInfo CreateInfo{};
		ETextureType TextureType = ETextureType::TEXTURE_2D;
	};

	struct FRenderbufferTexture
	{
		EFormat Format = EFormat::None;
	};

	struct BHIVE_API FramebufferAttachments
	{

		FramebufferAttachments() = default;
		FramebufferAttachments(std::initializer_list<FFramebufferTexture> attachments)
			: Attachments(attachments)
		{
		}

		FramebufferAttachments &reset()
		{
			Attachments.clear();
			mRenderBufferSpecification = {};
			return *this;
		}

		FramebufferAttachments &attach(const FTextureCreateInfo &create_info, ETextureType type = ETextureType::TEXTURE_2D)
		{
			Attachments.push_back(FFramebufferTexture{create_info, type});
			return *this;
		}

		FramebufferAttachments &attach(const FRenderbufferTexture &format)
		{
			mRenderBufferSpecification = format;
			return *this;
		}

		const std::vector<FFramebufferTexture> &GetAttachments() const { return Attachments; }

		const FRenderbufferTexture &GetRenderBuffer() const { return mRenderBufferSpecification; }

	private:
		std::vector<FFramebufferTexture> Attachments;
		FRenderbufferTexture mRenderBufferSpecification;

		friend class Framebuffer;
	};

	struct BHIVE_API FramebufferSpecification
	{
		FramebufferAttachments Attachments;
		glm::uvec2 Size{800, 600};
		uint32_t Depth = 1;
		uint32_t Samples = 1;
	};

	class BHIVE_API Framebuffer
	{
	public:

		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;

		virtual void UnBind() const = 0;

		virtual void Resize(const glm::uvec2 &newSize) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, const int *data) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, const float *data) = 0;

		virtual void Blit(const Ref<Framebuffer> &target) = 0;

		virtual void BlitToWindow(unsigned x, unsigned y, unsigned w, unsigned h) = 0;

		virtual void ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, void *data) const = 0;

		virtual uint32_t GetNumColorAttachments() const = 0;

		virtual Ref<Texture> GetColorAttachment(uint32_t index = 0) const = 0;

		virtual Ref<Texture> GetDepthAttachment() const = 0;

		virtual const FramebufferSpecification &GetSpecification() const = 0;

		virtual const glm::uvec2& GetSize() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification &specification);
	};

} // namespace BHive