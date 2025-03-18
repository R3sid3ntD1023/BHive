#pragma once

#include "core/Core.h"
#include "gui/ImGuiExtended.h"
#include "IWindow.h"

namespace BHive
{
	struct WindowNameCounter
	{
		std::string GetName(const std::string &name, uint32_t &id)
		{
			if (mUnUsedIDs.contains(name))
			{
				auto &ids = mUnUsedIDs.at(name);
				id = ids.front();
				ids.pop();
			}
			else
			{
				id = mCurrentID[name]++;
			}

			return name + "##" + std::to_string(id);
		}

		void RemoveName(const std::string &name, const uint32_t &id)
		{
			if (mCurrentID.contains(name))
			{
				mUnUsedIDs[name].push(id);
			}
		}

	private:
		std::unordered_map<std::string, uint32_t> mCurrentID;
		std::unordered_map<std::string, std::queue<uint32_t>> mUnUsedIDs;
	};

	struct WindowBase : public IWindow
	{
		WindowBase(uint32_t windowFlags = 0);

		virtual ~WindowBase();

		void OnUpdate() override;

		bool ShouldClose() const override { return !mIsOpen; };

		virtual bool IsFocused() const override { return mIsFocused || mIsHovered; }

	protected:
		virtual void OnGuiRender() {}

		virtual const char *GetName() const { return "Window"; }

	private:
		uint32_t mWindowFlags;

		bool mIsOpen{true};
		bool mIsFocused{true};
		bool mIsHovered{true};

		std::string mWindowIDStr;
		uint32_t mWindowID{0};

		static inline WindowNameCounter mCounter;
	};
} // namespace BHive
