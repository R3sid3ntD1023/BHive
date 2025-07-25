#pragma once

#include "WindowBase.h"
#include "core/UUID.h"

namespace BHive
{
	class AssetEditor : public WindowBase
	{
	public:
		AssetEditor(int flags = 0);

		virtual ~AssetEditor() = default;

		void OnGuiRender();

		bool IsOpened() const { return mIsOpened; }

		virtual void SetContext(const UUID &handle) {};

	protected:
		virtual bool OnSave(const std::filesystem::path &) const { return false; };

		virtual void OnMenuBar() {};

		virtual void OnWindowRender() = 0;

	protected:
		std::filesystem::path mCurrentSavePath;

	private:
		void MenuBar();

	private:
		bool mIsOpened = true;
	};
} // namespace BHive
