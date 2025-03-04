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
		{
			auto &reflection_data = mComputeShader->GetRelectionData();
			auto &buffer = reflection_data.UniformBuffers.at("Aces");
			mBuffer = CreateRef<UniformBuffer>(buffer.Binding, buffer.Size);
		}

		Initialize(w, h);
	}

	Ref<Texture> Aces::Process(const Ref<Texture> &texture)
	{
		struct FAcesData
		{
			uint64_t Src, Out;
		};

		FAcesData data{.Src = texture->GetResourceHandle(), .Out = mOutput->GetImageHandle()};
		mBuffer->SetData(&data, sizeof(FAcesData));

		mComputeShader->Bind();
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