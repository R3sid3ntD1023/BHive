#pragma once

#include "windows/ImWindowBase.h"
#include "core/UUID.h"

namespace BHive
{
	class AssetEditor : public ImWindowBase
	{
	public:
		AssetEditor(int flags = 0);

		virtual ~AssetEditor() = default;

		void OnUpdate() override;

		bool IsOpen() const { return mIsOpened; }

		virtual void SetContext(const UUID &handle) {};

	protected:
		virtual bool OnSave(const std::filesystem::path &) const { return false; };

		virtual void OnMenuBar() {};

		virtual void OnContentUpdate() = 0;

	protected:
		std::filesystem::path mCurrentSavePath;
		bool mIsOpened = true;
	};
} // namespace BHive
