#include "input/InputManager.h"
#include "InputContext.h"

namespace BHive
{
	template <>
	const float InputValue::Get() const
	{
		return mValue.x;
	}

	template <>
	const bool InputValue::Get() const
	{
		return (bool)mValue.x;
	}

	template <>
	const glm::vec2 InputValue::Get() const
	{
		return {mValue.x, mValue.y};
	}

	void InputContext::add_action(const std::string &name, FInputKey key)
	{
		mKeys.emplace_back(name, key);
	}

	bool InputContext::has_key(const std::string name) const
	{
		auto it = std::find_if(
			mKeys.begin(), mKeys.end(), [name](const FInputAction &action) { return action.GetName() == name; });

		return it != mKeys.end();
	}

	void InputContext::process()
	{
		auto &input = InputManager::GetInputManager();

		for (auto &action : mKeys)
		{
			glm::vec2 value = {0.f, 0.f};
			auto key = action.GetKey();

			if (mBindedAxisKeys.contains(action.GetName()))
			{
				switch (key)
				{
				case Mouse_X:
					value.x = input.get_mouse_delta().x;
					break;
				case Mouse_Y:
					value.y = input.get_mouse_delta().y;
					break;
				case Mouse_XY:
					value = input.get_mouse_delta();
					break;
				default:
					if (input.get_input_state(key) != EventStatus::RELEASE)
					{
						value.x = 1.0f;
					}
					break;
				}

				auto axis_callback = mBindedAxisKeys[action.mName];
				axis_callback.Callback(InputValue(value * axis_callback.Scale));
			}
			else if (mBindedKeys.contains(action.mName))
			{
				auto state = input.get_input_state(key);

				if (mBindedKeys.contains(action.mName))
				{
					auto &button_callback = mBindedKeys[action.mName];

					// LOG_INFO("InputContext:: State = {}, PrevoisState{}", state,
					// button_callback.PreviousState);

					if (state != button_callback.PreviousState)
					{
						bool handled = (state == button_callback.CallState);

						// LOG_TRACE("InputContext:: Call Callback = {}", handled ? "true" :
						// "false");

						if (handled)
						{
							button_callback.Callback(InputValue(handled));
							LOG_INFO("Handled");
						}

						button_callback.PreviousState = state;

						// LOG_INFO("InputContext:: State = {}, PrevoisState {}", state,
						// button_callback.PreviousState);
					}
				}
			}
		}
	}

	void InputContext::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mKeys);
	}

	void InputContext::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mKeys);
	}

	InputContext *InputContext::CreateInstance()
	{
		return new InputContext(*this);
	}

	REFLECT(InputContext)
	{
		{
			BEGIN_REFLECT(FInputAction) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Key", mKey) REFLECT_PROPERTY("Name", mName);
		}
		{
			BEGIN_REFLECT(InputContext) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Keys", mKeys);
		}
	}
} // namespace BHive
