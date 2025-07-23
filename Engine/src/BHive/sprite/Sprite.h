#pragma once

#include "asset/Asset.h"
#include "core/Core.h"
#include "gfx/textures/Texture2D.h"
#include "core/math/Math.h"

namespace BHive
{
	class Texture2D;

	struct FSpriteGenerator
	{
		glm::vec2 Coordinates{0, 0};

		glm::vec2 CellSize{1, 1};

		glm::vec2 Size{1, 1};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Coordinates, CellSize, Size);
		}
	};

	class BHIVE_API Sprite : public Asset
	{
	public:
		Sprite() = default;
		Sprite(const Sprite &other);

		Sprite(const Ref<Texture2D> &texture, const FSpriteGenerator &generator);

		Sprite(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max);

		void SetCoords(const glm::vec2 &min, const glm::vec2 &max);

		void SetFromGenerator(const FSpriteGenerator &generator);

		void Save(cereal::BinaryOutputArchive &ar) const;
		void Load(cereal::BinaryInputArchive &ar);

		const Ref<Texture2D> &GetSourceTexture() const { return mSource; }
		const glm::vec2 *GetCoords() const { return mCoords.data(); }
		const glm::vec2 &GetMinCoords() const { return mMin; }
		const glm::vec2 &GetMaxCoords() const { return mMax; }

		const FSpriteGenerator &GetGenerator() const { return mGenerator; }

		static Ref<Sprite> Create(const Ref<Texture2D> &texture, const FSpriteGenerator &generator);

		static Ref<Sprite> Create(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max);

		REFLECTABLEV(Asset)

	private:
		void Initialize();
		void CalculateMinMax(const FSpriteGenerator &generator);

	private:
		Ref<Texture2D> mSource;

		glm::vec2 mMin{0, 0};

		glm::vec2 mMax{1, 1};

		FSpriteGenerator mGenerator{};

		std::array<glm::vec2, 4> mCoords = {};
	};

} // namespace BHive
