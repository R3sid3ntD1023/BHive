#include "Lights.h"
#include "Renderer.h"
#include "gfx/Buffers.h"
#include "gfx/factories/BufferFactory.h"

namespace BHive
{
	constexpr uint32_t HeaderSize = 16;

	void Lights::Init()
	{
		mDirectionalLightBuffer = BufferFactory::Create(HeaderSize + sizeof(FGPUDirectionalLight) * sMaxLights, EBufferType::StorageBuffer);
		mLocalLightBuffer = BufferFactory::Create(HeaderSize + sizeof(FGPULocalLight) * sMaxLights, EBufferType::StorageBuffer);
	}

	void Lights::BeginRecording()
	{
		mDirectionalLights.clear();
		mLocalLights.clear();
	}

	void Lights::EndRecording()
	{
		auto dirBuffer = mDirectionalLightBuffer.As<GeneralBuffer>();
		auto localBuffer = mLocalLightBuffer.As<GeneralBuffer>();

		auto dirCount = mDirectionalLights.size();
		auto localCount = mLocalLights.size();

		dirBuffer->SetData(&dirCount, sizeof(uint32_t));
		localBuffer->SetData(&localCount, sizeof(uint32_t));

		dirBuffer->SetData(mDirectionalLights.data(), mDirectionalLights.size() * sizeof(FGPUDirectionalLight), HeaderSize);
		localBuffer->SetData(mLocalLights.data(), mLocalLights.size() * sizeof(FGPULocalLight), HeaderSize);
	}

	void Lights::Submit(const DirectionalLight &light)
	{
		mDirectionalLights.emplace_back(light.ToGPU());
	}

	void Lights::Submit(const PointLight &light)
	{
		mLocalLights.emplace_back(light.ToGPU());
	}

	void Lights::Submit(const SpotLight &light)
	{
		mLocalLights.emplace_back(light.ToGPU());
	}

} // namespace BHive