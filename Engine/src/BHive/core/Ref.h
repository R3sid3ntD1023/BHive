#pragma once

#include "core/Core.h"
#include "reflection/Reflection.h"

namespace BHive
{
	class Asset;

	struct RefBase
	{
		REFLECTABLEV()
	};

	struct RefCounter
	{
	public:
		RefCounter()
			: mCounter(0)
		{
		}

		RefCounter(const RefCounter &) = delete;
		RefCounter &operator=(const RefCounter &) = delete;

		~RefCounter() {}

		void reset() { mCounter = 0; }

		unsigned int get() { return mCounter; }

		void operator++() { mCounter++; }
		void operator++(int) { mCounter++; }

		void operator--() { mCounter--; }
		void operator--(int) { mCounter--; }

	private:
		unsigned int mCounter;
	};

	template <typename T>
	struct Ref : public RefBase
	{
		Ref(T *ptr = nullptr)
			: mPtr(ptr)
		{
			mCounter = new RefCounter();
			(*mCounter)++;
		}

		template <
			typename U,
			std::enable_if_t<std::is_base_of_v<T, U> || std::is_same_v<T, U>, bool> = true>
		Ref(U *ptr = nullptr)
			: mPtr(ptr)
		{
			mCounter = new RefCounter();
			(*mCounter)++;
		}

		template <
			typename U,
			std::enable_if_t<std::is_base_of_v<T, U> || std::is_same_v<T, U>, bool> = true>
		Ref(const Ref<U> &rhs)
			: mPtr(rhs.mPtr)
		{
			mCounter = rhs.mCounter;
			(*mCounter)++;
		}

		~Ref()
		{
			(*mCounter)--;
			if (mCounter->get() == 0)
			{
				delete mCounter;
				delete mPtr;
			}
		}

		unsigned int use_count() { return mCounter->get(); }

		T *get() const { return mPtr; }

		T *operator->() const { return mPtr; }

		T &operator*() const { return *mPtr; }

		template <
			typename U,
			std::enable_if_t<std::is_base_of_v<T, U> || std::is_same_v<T, U>, bool> = true>
		Ref &operator=(const U *&ptr)
		{
			mPtr = ptr;
			mCounter = new RefCounter();
			(*mCounter)++;
			return *this;
		}

		template <
			typename U,
			std::enable_if_t<std::is_base_of_v<T, U> || std::is_same_v<T, U>, bool> = true>
		Ref &operator=(const Ref<U> &rhs)
		{
			if (mPtr != rhs.mPtr)
			{
				if (mPtr && mCounter)
				{
					(*mCounter)--;

					if (mCounter->get() == 0)
					{
						delete mCounter;
						delete mPtr;
					}
				}
			}

			mPtr = rhs.mPtr;

			if (mPtr)
			{
				mCounter = rhs.mCounter;
				(*mCounter)++;
			}

			return *this;
		}

		operator bool() const { return mPtr != nullptr; }

		template <typename A>
		inline uint64_t SaveMinimal(const A &ar) const
		{
			return mPtr ? mPtr->GetHandle() : AssetHandle(0);
		}

		template <typename A>
		inline void LoadMinimal(const A &ar, const uint64_t &value)
		{
			mPtr = AssetManager::GetAsset<T>(value);
		}

		REFLECTABLEV(RefBase)

	private:
		T *mPtr;
		RefCounter *mCounter;
	};

	template <typename T, typename... TArgs>
	Ref<T> CreateRef(TArgs &&...args)
	{
		return Ref<T>(new T(std::forward<TArgs>(args)...));
	}
} // namespace BHive