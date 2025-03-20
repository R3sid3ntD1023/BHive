#include "Aces.h"
#include "gfx/Shader.h"
#include "gfx/UniformBuffer.h"
#include "shaders/AcesFilter.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	Aces::Aces(uint32_t w, uint32_t h)
	{
		mComputeShader = ShaderManager::Get().Load("Aces", aces_comp);

		Initialize(w, h);
	}

	Ref<Texture> Aces::Process(const Ref<Texture> &texture)
	{
		mComputeShader->Bind();
		texture->Bind();
		mOutput->BindAsImage(0, EImageAccess::WRITE);
		mComputeShader->Dispatch(texture->GetWidth(), texture->GetHeight());
		mComputeShader->UnBind();
		return mOutput;
	}

	void Aces::Resize(uint32_t w, uint32_t h)
	{
		mOutput.reset();

		Initialize(w, h);
	}

	void Aces::Initialize(uint32_t w, uint32_t h)
	{
		FTextureSpecification specs;
		specs.Channels = 3;
		specs.InternalFormat = EFormat::R11_G11_B10;
		specs.ImageAccess = EImageAccess::WRITE;
		mOutput = CreateRef<Texture2D>(w, h, specs);
	}
} // namespace BHive