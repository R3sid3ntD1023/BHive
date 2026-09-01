#pragma once

#include "RegistryManager.h"
#include "ResourceHandle.h"
#include "core/type/TypeID.h"

namespace BHive
{
	template <typename T>
	struct TResourcePtr
	{
		ResourceHandle Handle{};

		TResourcePtr() = default;

		TResourcePtr(const ResourceHandle &handle)
			: Handle(handle)
		{
		}

		ResourceHandle GetHandle() const { return Handle; }

		bool IsValid() const { return Handle.IsValid(); }

		template <typename Ar>
		void Serialize(Ar &ar, uint32_t)
		{
			ar(Handle);
		}

		TResourcePtr &operator=(const ResourceHandle &handle)
		{
			Handle = handle;
			return *this;
		}

		T *operator->() const { return static_cast<T *>(Get()); }

		void *Get()
		{
			auto reg = ResourceRegistriesManager::GetRegistry(Handle.Type);
			return reg ? reg->Get(Handle) : nullptr;
		}

		explicit operator bool() const { return IsValid(); }
	};
} // namespace BHive
