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

		void OnMenuBar() override;

		virtual void SetContext(const UUID &handle) {};

	protected:
		virtual bool OnSave(const std::filesystem::path &) { return false; };

	private:
		void SaveAs();
		void Save();

	protected:
		std::filesystem::path mCurrentSavePath;
	};
} // namespace BHive
