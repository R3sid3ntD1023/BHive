#pragma once

#include "input/InputContext.h"
#include "scene/Component.h"

namespace BHive
{

	struct BHIVE InputComponent : public Component
	{

		TAssetHandle<InputContext> mInputContext;

		virtual void OnBegin() override;

		virtual void OnUpdate(float) override;

		virtual void OnEnd() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		InputContext *GetContext() const { return mContextInstance; }

	private:
		InputContext *mContextInstance = nullptr;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(InputComponent)

} // namespace BHive
