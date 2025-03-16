#pragma once

#include "Component.h"
#include "input/InputContext.h"

namespace BHive
{

	struct InputComponent : public Component
	{

		TAssetHandle<InputContext> mInputContext;

		void CreateInstance();

		InputContext *GetContext() const { return mContextInstance; }

		void DestroyInstance();

		void Begin() override;
		void Update(float) override;
		void End() override;

	private:
		InputContext *mContextInstance = nullptr;
	};

} // namespace BHive
