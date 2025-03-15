#pragma once

#include "input/InputContext.h"

namespace BHive
{

	struct InputComponent
	{

		TAssetHandle<InputContext> mInputContext;

		void CreateInstance();

		InputContext *GetContext() const { return mContextInstance; }

		void DestroyInstance();

	private:
		InputContext *mContextInstance = nullptr;
	};

} // namespace BHive
