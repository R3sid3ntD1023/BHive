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
		Framebuffer(const FramebufferSpecification &specification);
		virtual ~Framebuffer();

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual void Resize(uint32_t width, uint32_t height);
		virtual void ClearAttachment(uint32_t attachmentIndex, unsigned type, const float *data);
		virtual void Blit(Ref<Framebuffer> &target);

		virtual void
		ReadPixel(uint32_t attachmentIndex, unsigned x, unsigned y, unsigned w, unsigned h, unsigned type, void *data) const;

		virtual Ref<Texture> GetColorAttachment(uint32_t index = 0) const;
		virtual Ref<Texture> GetDepthAttachment() const;

		virtual const FramebufferSpecification &GetSpecification() const { return mSpecification; }
		virtual uint32_t GetWidth() const { return mSpecification.Width; }
		virtual uint32_t GetHeight() const { return mSpecification.Height; }

		virtual uint32_t GetRendererID() const { return mFramebufferID; }
		operator uint32_t() const { return GetRendererID(); }

	private:
		virtual void Initialize();
		virtual void Release();

	private:
		std::vector<FFramebufferTexture> mColorSpecifications{};
		FFramebufferTexture mDepthSpecification{};

		std::vector<Ref<Texture>> mColorAttachments;
		Ref<Texture> mDepthAttachment;

		FramebufferSpecification mSpecification;

		uint32_t mFramebufferID{0};
		uint32_t mRenderBufferID{0};
	};

} // namespace BHive