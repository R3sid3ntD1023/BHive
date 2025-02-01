#pragma once

#include "core/Core.h"
#include "asset/Asset.h"
#include "events/InputAction.h"
#include "input/InputAction.h"
#include "input/InputValue.h"
#include "core/EventDelegate.h"

namespace BHive
{
	class InputContext : public Asset
	{
	public:
		struct FInputActionCallback
		{
			EventDelegate<const InputValue &> Callback;
			InputActionCode CallState = InputAction::RELEASE;
			InputActionCode PreviousState = InputAction::RELEASE;
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
		void bind_key(const std::string &name, InputActionCode action, T *instance, void (T::*func)(const InputValue &value))
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
