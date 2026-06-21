#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "importers/TextureImporter.h"
#include "OutlineRenderPass.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	void OutlineRenderPass::Init()
	{
		mOutlineMeshShader = ShaderManager::Get("Outline.glsl");
		mOutlineQuadShader = ShaderManager::Get("OutlineQuad.glsl");
	}

	void OutlineRenderPass::Render(const FMeshRenderDatas &data)
	{
		mFrambuffer->Bind();

		Renderer::Get().ClearColor(0, 0, 0, 0);
		Renderer::Get().Clear();


		/*switch (mSelectedRenderData->GetRenderDataType())
		{
		case FMeshRenderData::Billboard:
			mOutlineQuadShader->Bind();
			break;
		default:
			mOutlineMeshShader->Bind();
			break;
		}*/

		//Renderer::Draw(mSelectedRenderData);

		mFrambuffer->UnBind();
	}

	void OutlineRenderPass::CreateFramebuffer()
	{
		FramebufferSpecification specs{};
		specs.Size = mSize;
		specs.Attachments.attach(FTextureCreateInfo{.Format = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE}, ETextureType::TEXTURE_2D);
		specs.Attachments.attach(FRenderbufferTexture{.Format = EFormat::DEPTH24_STENCIL8});
		mFrambuffer = Framebuffer::Create(specs);
	}

	void OutlineRenderPass::SetSelected(const Ref<FMeshRenderData> &data)
	{
		mSelectedRenderData = data;
	}

	Ref<class Texture> OutlineRenderPass::GetOutputTetxure() const
	{
		return mFrambuffer->GetColorAttachment(0);
	}

	void OutlinePostProcessRenderPass::Init()
	{
		mOutlineColorGradingShader = ShaderManager::Get("OutlineColorGrading.glsl");
		mBoxBlurShader = ShaderManager::Get("BoxBlur.glsl");

		FTextureCreateInfo create_info_lut{};
		create_info_lut.Format = EFormat::RGBA32F;
		create_info_lut.WrapMode = EWrapMode::REPEAT;
		create_info_lut.MinFilter = EMinFilter::LINEAR;

		int32_t width = 0, height = 0, channels = 0;
		uint8_t *data = nullptr;
		bool loaded = TextureLoader::LoadImageData(ENGINE_PATH "/data/textures/cg_none.png", width, height, channels, data);

		if (loaded)
		{
			mColorGradingLUTTexture = Texture3D::Create({width, width, height / width}, create_info_lut, Buffer(data, width * height * channels));
			free(data);
		}
	}

	void OutlinePostProcessRenderPass::CreateResizableObjects(const glm::uvec2 &size)
	{
		PostProcessRenderPass::CreateResizableObjects(size);

		FTextureCreateInfo create_info{};
		create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		create_info.Format = EFormat::RGBA32F;

		mOutputTexture = Texture2D::Create(size, create_info);
		mOutlineOutput = Texture2D::Create(size, create_info);
	}

	void OutlinePostProcessRenderPass::Process(const Ref<Texture> &texture)
	{
		/*Image output_image(mOutputTexture);
		Image outline_image(mOutlineOutput);*/

		//mBoxBlurShader->Bind();
		//mOutlineTexture->Bind(0);
		//outline_image.Bind(0, EImageAccess::WRITE);
		//mBoxBlurShader->Dispatch(mSize.x, mSize.y);
		//mBoxBlurShader->UnBind();

		//mOutlineColorGradingShader->Bind();

		//texture->Bind(0);		 // color sampler
		//mOutlineOutput->Bind(1); // blurred outline texture
		//mColorGradingLUTTexture->Bind(2);

		//output_image.Bind(0, EImageAccess::WRITE);

		/*mOutlineColorGradingShader->Dispatch(mSize.x, mSize.y);
		mOutlineColorGradingShader->UnBind();*/
	}

	void OutlinePostProcessRenderPass::SetSelected(bool selected)
	{
		mIsSelected = selected;
	}

	void OutlinePostProcessRenderPass::SetOutlineTexture(const Ref<Texture> &outline_texture)
	{
		mOutlineTexture = outline_texture;
	}
} // namespace BHive