#include "Aces.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/Buffers.h"

namespace BHive
{

	void AcesRenderPass::Process(const Ref<Texture> &texture)
	{
		//Image image(mOutputTexture);

		/*mComputeShader->Bind();
		texture->Bind();
		image.Bind(0, EImageAccess::WRITE);
		mComputeShader->Dispatch(texture->GetWidth(), texture->GetHeight());
		mComputeShader->UnBind();*/
	}

	void AcesRenderPass::Init()
	{
		mComputeShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/compute/Aces.glsl");
	}

	void AcesRenderPass::CreateResizableObjects(const glm::uvec2 &size)
	{
		PostProcessRenderPass::CreateResizableObjects(size);

		FTextureCreateInfo specs{};
		specs.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		specs.Format = EFormat::RGBA8;

		mOutputTexture = Texture2D::Create({mSize.x, mSize.y}, specs);
	}
} // namespace BHive