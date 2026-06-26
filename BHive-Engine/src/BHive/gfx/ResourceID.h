#pragma once

#include "core/Core.h"

namespace BHive
{
	struct IDPool
	{
	public:
		uint32_t Aquire()
		{
			if (!mFreeList.empty())
			{
				uint32_t id = mFreeList.back();
				mFreeList.pop_back();
				return id;
			}

			return mCounter.fetch_add(1, std::memory_order_relaxed);
		}

		void Release(uint32_t id) { mFreeList.push_back(id); }

	private:
		std::atomic<uint32_t> mCounter{1};
		std::vector<uint32_t> mFreeList;
	};

	template<typename PoolTag>
	class TResourceID
	{
	public:
		TResourceID()
			: mID(GetPool().Aquire())
		{}

		explicit TResourceID(uint32_t id)
			: mID(id)
		{
		}

		void Release()
		{
			if (mID == 0)
				return;

			GetPool().Release(mID);
			mID = 0;
		}

		operator uint32_t() const { return mID; }
		operator bool() const { return mID != 0; }

		bool operator==(const TResourceID &rhs) const { return mID == rhs.mID; }
		bool operator!=(const TResourceID &rhs) const { return mID != rhs.mID; }

	private:
		uint32_t mID = 0;

		static IDPool &GetPool();

		friend struct std::hash<TResourceID<PoolTag>>;
	};

	
	struct EnginePoolTag
	{
	};

	struct GPUPoolTag
	{
	};

	template <>
	inline IDPool &TResourceID<EnginePoolTag>::GetPool()
	{
		static IDPool sEnginePool;
		return sEnginePool;
	}

	template <>
	inline IDPool &TResourceID<GPUPoolTag>::GetPool()
	{
		static IDPool sGPUPool;
		return sGPUPool;
	}

	using EngineResourceID = TResourceID<EnginePoolTag>;
	using ResourceID = TResourceID<GPUPoolTag>;

	
} // namespace BHive

namespace std
{
	template<typename Tag>
	struct hash<BHive::TResourceID<Tag>>
	{
		size_t operator()(const BHive::TResourceID<Tag> &id) const { return std::hash<uint32_t>()((uint32_t)id); }
	};
}

namespace fmt
{
	template <typename Tag>
	struct formatter<BHive::TResourceID<Tag>>
	{
		constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const BHive::TResourceID<Tag> &id, FormatContext &ctx) const
		{
			return fmt::format_to(ctx.out(), "{}", (uint32_t)id);
		}
	};

}