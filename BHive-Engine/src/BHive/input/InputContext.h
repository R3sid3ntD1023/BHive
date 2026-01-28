#pragma once

#include "asset/Asset.h"
#include "core/Core.h"
#include "core/EventDelegate.h"
#include "core/events/EventStatus.h"
#include "input/InputAction.h"
#include "input/InputValue.h"

namespace BHive
{

	class BHIVE_API InputContext : public Asset
	{
	public:
		InputContext() = default;

		struct FInputActionCallback
		{
			EventDelegate<const InputValue &> Callback;
			EventStatusCode CallState = EventStatus::RELEASE;
			EventStatusCode PreviousState = EventStatus::RELEASE;
			float Scale = 1.0f;
		};

		struct FInputAxisCallback
		{
			EventDelegate<const InputValue &> Callback;
			float Scale = 1.0f;
		};

		void process();

		void add_action(const std::string &name, FInputKey key);

		template <typename T>
		void bind_key(const std::string &name, EventStatusCode action, T *instance, void (T::*func)(const InputValue &value))
		{
			if (has_key(name))
			{
				auto &binded_keys = mBindedKeys[name];
				binded_keys.CallState = action;
				binded_keys.Callback.bind(instance, func);
			}
		}

		template <typename T>
		void bind_axis(const std::string &name, T *instance, void (T::*func)(const InputValue &value), float scale = 1.0f)
		{
			if (has_key(name))
			{
				auto &binded_axis = mBindedAxisKeys[name];
				binded_axis.Scale = scale;
				binded_axis.Callback.bind(instance, func);
			}
		}

		bool has_key(const std::string name) const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		InputContext *CreateInstance();

		std::vector<FInputAction> mKeys;
		std::unordered_map<std::string, FInputActionCallback> mBindedKeys;
		std::unordered_map<std::string, FInputAxisCallback> mBindedAxisKeys;

		friend struct InputComponent;
		friend class InputContextSerializer;
	};

	REFLECT_EXTERN(InputContext)

} // namespace BHive
