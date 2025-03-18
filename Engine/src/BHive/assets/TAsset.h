#pragma once

#include "asset/Asset.h"

namespace BHive
{
	template <typename T>
	class TAsset : public Asset
	{
	protected:
		Ref<T> mObject;

	public:
		TAsset() = default;
		TAsset(const Ref<T> &obj)
			: mObject(obj)
		{
		}

		Ref<T> Get() const { return mObject; }

		Ref<T> operator->() const { return Get(); }

		operator Ref<T>() const { return Get(); }

		REFLECTABLEV(Asset)
	};

} // namespace BHive
