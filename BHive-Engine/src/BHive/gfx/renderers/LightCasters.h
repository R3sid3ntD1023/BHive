#pragma once

#include "gfx/Color.h"

namespace BHive
{
	
	struct FGPUDirectionalLight
	{
		// rgb + intensity
		glm::vec4 Color;

		// xyz + unused
		glm::vec4 Direction;
	};

	struct FGPUPointLight
	{
		// rgb + intensity
		glm::vec4 Color;

		// xyz + radius
		glm::vec4 Position;
	};

	struct FGPUSpotLight
	{
		// rgb + intensity
		glm::vec4 Color;

		// xyz + radius
		glm::vec4 Position;

		// xyz + innerCutoff
		glm::vec4 Direction;

		// outerCutoff + padding
		glm::vec4 Params;
	};

	class BHIVE_API DirectionalLight
	{
	public:
		DirectionalLight &SetColor(const FColor &color);

		DirectionalLight &SetIntensity(float intensity);

		DirectionalLight &SetDirection(const glm::vec3 &direction);

		const FColor& GetColor() { return mColor; }

		const glm::vec3& GetDirection() const { return mDirection; }

		float GetIntensity() const { return mIntensity; }

		FGPUDirectionalLight ToGPU() const;

		REFLECTABLE()

	private:
		FColor mColor = FColor::White;
		glm::vec3 mDirection = {-1, 0, 0};
		float mIntensity = 1.f;
	};

	class BHIVE_API PointLight
	{
	public:
		PointLight &SetColor(const FColor &color);

		PointLight &SetIntensity(float intensity);

		PointLight &SetPosition(const glm::vec3 &position);

		PointLight &SetRadius(float radius);

		const FColor& GetColor() { return mColor; }

		const glm::vec3& GetPosition() const { return mPosition; }

		float GetIntensity() const { return mIntensity; }

		float GetRadius() const { return mRadius; }
	
		FGPUPointLight ToGPU() const;

		REFLECTABLE()

	private:
		FColor mColor = FColor::White;
		glm::vec3 mPosition {0, 0, 0};
		float mIntensity = 1.f;
		float mRadius = 5.f;
	};

	class BHIVE_API SpotLight
	{
	public:
		SpotLight &SetColor(const FColor &color);

		SpotLight &SetIntensity(float intensity);

		SpotLight &SetPosition(const glm::vec3 &position);

		SpotLight &SetDirection(const glm::vec3 &direction);

		SpotLight &SetRadius(float radius);

		SpotLight &SetInnerAngleDegrees(float degrees);

		SpotLight &SetOuterAngleDegrees(float degrees);

		const FColor& GetColor() { return mColor; }

		const glm::vec3& GetDirection() const { return mDirection; }

		const glm::vec3& GetPosition() const { return mPosition; }

		float GetIntensity() const { return mIntensity; }

		float GetRadius() const { return mRadius; }

		float GetInnerAngleDegrees() const { return mInnerAngleDegrees; }

		float GetOuterAngleDegrees() const { return mOuterAngleDegrees; }

		REFLECTABLE()

		FGPUSpotLight ToGPU() const;

	private:
		FColor mColor = FColor::White;
		glm::vec3 mPosition{0, 0, 0};
		glm::vec3 mDirection{0, -1, 0};
		float mIntensity = 1.f;
		float mRadius = 5.f;
		float mInnerAngleDegrees = 20.f;
		float mOuterAngleDegrees = 30.f;
		float mInnerCutOffCos = glm::cos(glm::radians(mInnerAngleDegrees));
		float mOuterCutOffCos = glm::cos(glm::radians(mOuterAngleDegrees));
	};

	REFLECT_EXTERN(DirectionalLight);
	REFLECT_EXTERN(PointLight);
	REFLECT_EXTERN(SpotLight);

}