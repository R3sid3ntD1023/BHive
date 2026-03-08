#include "GlobalBuffers.h"
#include "gfx/UniformBuffer.h"
#include "gfx/StorageBuffer.h"

namespace BHive
{

	void GlobalBuffers::Register(uint32_t binding, const Ref<BufferBase> &buffer)
	{
		mBuffers.emplace(binding, buffer);
	}

	void GlobalBuffers::Register(uint32_t binding, const Ref<Texture> & texture)
	{
		mTextures.emplace(binding, texture);
	}

	bool GlobalBuffers::Contains(uint32_t binding) const
	{
		return mBuffers.contains(binding);
	}


} // namespace BHive