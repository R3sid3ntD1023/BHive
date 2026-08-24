#pragma once

#include "core/Core.h"

namespace BHive
{
	enum class EEventPriority : uint8_t
	{
		HIGH,
		NORMAL,
		LOW
	};

	template <typename... TArgs>
	struct DelegateEntry
	{
		EEventPriority Priority;
		std::function<void(void *, TArgs...)> Invoke;
		void *Instance = nullptr;
		std::weak_ptr<void> WeakPtr;
		bool IsWeak = false;
		uint64_t Handle = 0;
	};

	using MultiEventHandle = uint64_t;

	template <typename... TArgs>
	class MultiEventDelegate
	{
	public:
		using Entry = DelegateEntry<TArgs...>;

		MultiEventDelegate() = default;

		template <typename T>
		MultiEventHandle Add(T *instance, void (T::*func)(TArgs...), EEventPriority prio = EEventPriority::NORMAL)
		{
			Entry e;
			e.Priority = prio;
			e.Instance = instance;
			e.IsWeak = false;

			e.Invoke = [func](void *obj, TArgs... args) -> void { (static_cast<T *>(obj)->*func)(args...); };
			e.Handle = GenerateNextHanldle();
			mEntries.emplace_back(e);
			Sort();

			return e.Handle;
		}

		template <typename T>
		MultiEventHandle AddWeak(std::shared_ptr<T> instance, void (T::*func)(TArgs...), EEventPriority prio = EEventPriority::NORMAL)
		{
			Entry e;
			e.Priority = prio;
			e.Instance = instance.get();
			e.WeakPtr = instance;
			e.IsWeak = true;

			e.Invoke = [func](void *obj, TArgs... args) -> void { (static_cast<T *>(obj)->*func)(args...); };

			mEntries.emplace_back(e);
			Sort();
			return e.Handle;
		}

		void Remove(MultiEventHandle handle)
		{
			mEntries.erase(std::remove_if(mEntries.begin(), mEntries.end(), [handle](const auto &e) { return e.Handle == handle; }), mEntries.end());
		}

		void Broadcast(TArgs... args)
		{
			std::vector<Entry> snapshot;
			{
				CleanupExpired();
				snapshot = mEntries; // copy for lock free iteration
			}

			for (auto &e : snapshot)
			{
				e.Invoke(e.Instance, args...);
			}
		}

	private:
		std::vector<Entry> mEntries;
		static inline std::atomic<uint64_t> sNextHandle = 1;
		uint64_t GenerateNextHanldle() { return sNextHandle++; }

		void Sort()
		{
			std::sort(mEntries.begin(), mEntries.end(), [](const Entry &a, const Entry &b) { return (int)a.Priority < (int)b.Priority; });
		}

		void CleanupExpired()
		{
			mEntries.erase(std::remove_if(mEntries.begin(), mEntries.end(), [](const Entry &e) { return e.IsWeak && e.WeakPtr.expired(); }), mEntries.end());
		}
	};
} // namespace BHive

#define DECLARE_MULTI_EVENT(name, ...)                                   \
	struct name##Event : public ::BHive::MultiEventDelegate<__VA_ARGS__> \
	{                                                                    \
		name##Event() = default;                                         \
	};
