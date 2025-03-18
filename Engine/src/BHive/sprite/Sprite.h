#pragma once

#include "asset/Asset.h"
#include "core/Core.h"
#include "gfx/textures/Texture2D.h"
#include "math/Math.h"

namespace BHive
{
	class Texture2D;

	class Sprite : public Asset
	{
	public:
		Sprite() = default;
		Sprite(const Sprite &other);

		Sprite(
			const Ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 spriteSize);

		Sprite(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max);

		const Ref<Texture2D> &GetSourceTexture() const { return mSource; }
		const glm::vec2 *GetCoords() const { return mCoords.data(); }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		static Ref<Sprite> Create(
			const Ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 spriteSize);

		static Ref<Sprite> Create(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max);

		REFLECTABLEV(Asset)

	private:
		void Initialize();
		void CalculateMinMax(const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 spriteSize);

	private:
		TAssetHandle<Texture2D> mSource;
		std::array<glm::vec2, 4> mCoords = {};
		glm::vec2 mMin{0, 0}, mMax{1, 1};
	};

	REFLECT_EXTERN(Sprite)

} // namespace BHive
