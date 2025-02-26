#include "Aces.h"
#include "gfx/Shader.h"
#include "gfx/UniformBuffer.h"
#include "shaders/AcesFilter.h"
#include "gfx/Texture.h"

namespace BHive
{
	Aces::Aces(uint32_t w, uint32_t h)
	{
		mComputeShader = ShaderLibrary::Load("Aces", aces_comp);
		{
			auto &reflection_data = mComputeShader->GetRelectionData();
			auto &buffer = reflection_data.UniformBuffers.at("Aces");
			mBuffer = UniformBuffer::Create(buffer.Binding, buffer.Size);
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
		mBuffer->SetData(data);

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
		specs.mChannels = 3;
		specs.mFormat = EFormat::R11_G11_B10;
		specs.ImageAccess = EImageAccess::WRITE;
		mOutput = Texture2D::Create(w, h, specs);
	}
} // namespace BHive