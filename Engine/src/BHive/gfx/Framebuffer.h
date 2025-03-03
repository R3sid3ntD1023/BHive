#pragma once

#include "core/Core.h"
#include "Texture.h"
#include "math/Math.h"

namespace BHive
{
	class FramebufferTexture2D;
	class TextureCubeMap;

	struct FFramebufferTexture
	{
		FTextureSpecification mSpecification{};
		ETextureType TextureType = ETextureType::TEXTURE_2D;
	};

	struct BHIVE FramebufferAttachments
	{
		FramebufferAttachments() = default;
		FramebufferAttachments(std::initializer_list<FFramebufferTexture> attachments)
			: Attachments(attachments)
		{
		}

		FramebufferAttachments &reset()
		{
			Attachments.clear();
			return *this;
		}

		FramebufferAttachments &
		attach(const FTextureSpecification &specification, ETextureType type = ETextureType::TEXTURE_2D)
		{
			Attachments.push_back(FFramebufferTexture{specification, type});
			return *this;
		}

		const std::vector<FFramebufferTexture> &GetAttachments() const { return Attachments; }

	protected:
		std::vector<FFramebufferTexture> Attachments;

		friend class Framebuffer;
	};

	struct BHIVE FramebufferSpecification
	{
		FramebufferAttachments Attachments;
		FFramebufferTexture mRenderSpecification;
		uint32_t Width = 800, Height = 600, Depth = 1;
		uint32_t Samples = 1;
		bool WriteOnly = false;
	};

	class BHIVE Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, unsigned type, const float *data) = 0;
		virtual void Blit(Ref<Framebuffer> &target) = 0;

		virtual void ReadPixel(
			uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data) const = 0;

		virtual Ref<Texture> GetColorAttachment(uint32_t index = 0) const = 0;
		virtual Ref<Texture> GetDepthAttachment() const = 0;

		virtual const FramebufferSpecification &GetSpecification() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetRendererID() const = 0;
		operator uint32_t() const { return GetRendererID(); }

		static Ref<Framebuffer> Create(const FramebufferSpecification &specification);
	};

} // namespace BHive