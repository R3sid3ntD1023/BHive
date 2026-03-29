#include "RenderTargetCube.h"
#include "gfx/cameras/CubeCamera.h"
#include "gfx/Framebuffer.h"
#include "gfx/Texture.h"


namespace BHive
{

	RenderTargetCube::RenderTargetCube(uint32_t size, EFormat format)
		: mSize(size)
	{
		FTextureCreateInfo create_info{};
		create_info.Format = format;
		create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		create_info.MinFilter = EMinFilter::LINEAR;
		create_info.ArrayLayers = 6;
		create_info.GenerateMipMaps = 1;
		create_info.DebugName = "TargetTexture";
		create_info.Usage |= ETextureUsage::ColorAttachment | ETextureUsage::Storage;
		mTargetTexture = TextureCube::Create(size, create_info);

		FFramebufferTexture color{};
		color.ExistingTexture = mTargetTexture;
		color.Layer = 0;
		color.LayerCount = 6;
		color.TextureType = ETextureType::TEXTURE_CUBE_MAP;
		color.CreateInfo = create_info;

		FramebufferSpecification specification{};
		specification.Size = {size, size};
		specification.Attachments.attach(color);

		mFrameBuffer = Framebuffer::Create(specification);

	}

	void RenderTargetCube::Bind(uint32_t face)
	{
		mFrameBuffer->BindFace(face);
		mFrameBuffer->Bind();
	}

	void RenderTargetCube::UnBind()
	{
		mFrameBuffer->UnBind();
	}

	const Ref<Texture> RenderTargetCube::GetTargetTexture() const
	{
		return mTargetTexture;
	}

} // namespace BHive