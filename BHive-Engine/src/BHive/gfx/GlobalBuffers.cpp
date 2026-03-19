#include "GlobalBuffers.h"
#include "Platform/Vulkan/systems/GlobalSetRegistry.h"

namespace BHive
{
	void GlobalBuffers::UpdateCPU()
	{
	}
	void GlobalBuffers::Upload()
	{
	}
	void GlobalBuffers::Register(uint32_t binding, const Ref<BufferBase> &buffer)
	{
		mBuffers.emplace(binding, buffer);
	}

	void GlobalBuffers::Register(uint32_t binding, const Ref<Texture> & texture)
	{
		mTextures.emplace(binding, texture);
	}

} // namespace BHive