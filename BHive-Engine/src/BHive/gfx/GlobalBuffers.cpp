#include "GlobalBuffers.h"
#include "gfx/UniformBuffer.h"
#include "gfx/StorageBuffer.h"

namespace BHive
{
	const NativeHandle GlobalBuffers::BufferHandle::GetHandle(uint32_t frame) const
	{
		if (Buffer)
			return Buffer->GetNativeHandle(frame);

		return {};
	}

	void GlobalBuffers::Register(uint32_t binding, const BufferHandle &buffer)
	{
		mBuffers.emplace(binding, buffer);
	}

	bool GlobalBuffers::Contains(uint32_t binding) const
	{
		return mBuffers.contains(binding);
	}

} // namespace BHive