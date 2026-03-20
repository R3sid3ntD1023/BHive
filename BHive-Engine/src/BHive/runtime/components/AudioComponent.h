#pragma once

#include "runtime/Component.h"

namespace BHive
{
	class AudioSource;

	struct BHIVE_API AudioComponent : public Component
	{
	public:
		void Begin() override;
		void End() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		bool AutoPlay{false};

		Ref<AudioSource> Audio;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(AudioComponent)
} // namespace BHive