#pragma once

#include "scene/Component.h"
#include "input/InputContext.h"

namespace BHive
{

	struct BHIVE InputComponent : public Component
	{

		TAssetHandle<InputContext> mInputContext;

		virtual void OnBegin() override;

		virtual void OnUpdate(float) override;

		virtual void OnEnd() override;

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		InputContext *GetContext() const { return mContextInstance; }

	private:
		InputContext *mContextInstance = nullptr;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(InputComponent)

} // namespace BHive
