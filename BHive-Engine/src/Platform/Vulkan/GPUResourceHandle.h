#pragma once

#include <cstdint>
#include <functional>

namespace BHive
{
	struct GPUResourceHandle
	{
		virtual ~GPUResourceHandle() = default;

		GPUResourceHandle()
			: handle(nextHandle++)
		{
		}

		GPUResourceHandle(const GPUResourceHandle &other)
			: handle(other.handle)
		{
		}

		virtual void Destroy() { handle = 0; };

		operator bool() const { return handle != 0; }

		bool operator<(const GPUResourceHandle &other) const { return handle < other.handle; }

		bool operator==(const GPUResourceHandle &other) const { return handle == other.handle; }

		GPUResourceHandle &operator=(const GPUResourceHandle &other)
		{
			handle = other.handle;
			return *this;
		}

	private:
		uint64_t handle = 0;
		static inline uint64_t nextHandle = 1;
		friend struct std::hash<BHive::GPUResourceHandle>;
	};

	struct GPUBufferResourceHandle : public GPUResourceHandle
	{
		struct GPUBufferResource *Resolve();

		struct GPUBufferResource *operator->() { return Resolve(); };

		void Destroy() override;
	};

	struct GPUImageResourceHandle : public GPUResourceHandle
	{
		struct GPUImageResource *Resolve();

		struct GPUImageResource *operator->() { return Resolve(); };

		void Destroy() override;
	};

} // namespace BHive

namespace std
{
	template <>
	struct hash<BHive::GPUResourceHandle>
	{
		std::size_t operator()(const BHive::GPUResourceHandle &handle) const noexcept { return std::hash<uint64_t>{}(handle.handle); }
	};
} // namespace std