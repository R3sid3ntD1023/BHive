#pragma once

namespace BHive
{
	struct NativeHandle
	{
		uintptr_t Handle = 0;

		static NativeHandle FromRaw(uint64_t handle)
		{ 
			return {handle};
		}

		template<typename T>
		static NativeHandle FromPtr(const T* ptr)
		{
			return
			{
				reinterpret_cast<uintptr_t>(ptr)
			};
		}

		template <typename T>
		T *As()
		{
			return reinterpret_cast<T *>(Handle);
		}

		template<typename T>
		const T* As() const
		{
			return reinterpret_cast<const T *>(Handle);
		}

		uint64_t AsRaw() const { return Handle;}

		bool IsValid() const { return Handle != 0; }
	};

}