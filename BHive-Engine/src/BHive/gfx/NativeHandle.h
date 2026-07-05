#pragma once

namespace BHive
{
	struct INativeObject
	{
		virtual ~INativeObject() = default;
	};

	struct NativeHandle
	{
		enum class EKind
		{
			Handle,
			Ptr
		};

		EKind kind;
		uint64_t handle = 0;
		INativeObject *ptr = nullptr;

		static NativeHandle FromRaw(uint64_t handle)
		{ 
			return {EKind::Handle, handle, nullptr};
		}

		static NativeHandle FromPtr(const INativeObject* ptr)
		{
			return
			{
				EKind::Ptr, 0u, const_cast<INativeObject*>(ptr)
			};
		}

		template<typename T>
		bool Is() const
		{
			if constexpr (std::is_base_of_v<INativeObject, T>)
			{
				return (kind == EKind::Ptr) && (dynamic_cast<T *>(ptr) != nullptr);
			}

			return (kind == EKind::Handle);
		}

		template<typename T>
		T* As() const
		{
			if constexpr( std::is_base_of_v<INativeObject, T> )
			{
				return (kind == EKind::Ptr) ? dynamic_cast<T *>(ptr) : nullptr;
			}

			return  reinterpret_cast<T *>(handle);
		}

		uint64_t AsRaw() const { return handle;}

		bool IsValid() const { return (kind == EKind::Ptr) ? ptr != nullptr : handle != 0; }
	};

}