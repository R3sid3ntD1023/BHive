#pragma once

#include "Command.h"
#include <rttr/variant.h>

namespace BHive
{
	template <typename T>
	struct TCommandProperty : public ICommand
	{
		TCommandProperty(T *obj, const T &new_value)
			: mProperty(obj),
			  mOldValue(*obj),
			  mValue(new_value)
		{
		}

		TCommandProperty(const TCommandProperty &other)
			: mProperty(other.mProperty),
			  mOldValue(other.mOldValue),
			  mValue(other.mValue)
		{
		}

		virtual void OnUndo() override { *mProperty = mOldValue; }

		virtual void OnRedo() override { *mProperty = mValue; }

		virtual bool CanMerge() const { return true; }

		virtual bool Merge(const std::shared_ptr<ICommand> &other) override
		{
			auto command = std::dynamic_pointer_cast<TCommandProperty<T>>(other);
			if (command)
			{
				if (command->mProperty == mProperty && mOldValue == command->mOldValue)
				{
					command->mValue = mValue;
					return true;
				}

				return false;
			}
		}

	private:
		T *mProperty;
		T mValue;
		T mOldValue;
	};

	struct CommandProperty : public ICommand
	{
		CommandProperty(rttr::variant obj, rttr::property prop, const rttr::variant &new_value)
			: mInstance(obj),
			  mProperty(prop),
			  mOldValue(mProperty.get_value(mInstance)),
			  mValue(new_value)
		{
		}

		CommandProperty(const CommandProperty &other)
			: mInstance(other.mInstance),
			  mProperty(other.mProperty),
			  mOldValue(other.mOldValue),
			  mValue(other.mValue)
		{
		}

		virtual void OnUndo() override { mProperty.set_value(mInstance, mOldValue); }

		virtual void OnRedo() override { mProperty.set_value(mInstance, mValue); }

		virtual bool CanMerge() const { return true; }

		virtual bool Merge(const std::shared_ptr<ICommand> &other) override
		{
			auto command = std::dynamic_pointer_cast<CommandProperty>(other);
			if (command)
			{
				if (command->mProperty == mProperty && mOldValue == command->mOldValue)
				{
					command->mValue = mValue;
					return true;
				}

				return false;
			}
		}

	private:
		rttr::instance mInstance;
		rttr::property mProperty;
		rttr::variant mValue;
		rttr::variant mOldValue;
	};

} // namespace BHive