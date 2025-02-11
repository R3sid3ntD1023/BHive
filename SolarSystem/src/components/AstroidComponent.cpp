#include "AstroidComponent.h"
#include "CelestrialBody.h"

float GetRandomDisplacement(float offset)
{
	return (rand() % (int)(2 * offset * 100)) / 100.f - offset;
}

void AstroidComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP("Instances", mNumInstances), MAKE_NVP("Seed", mSeed), MAKE_NVP("Radius", mRadius), MAKE_NVP("Offset", mOffset),
	   MAKE_NVP("Meshes", mMeshHandles), MAKE_NVP("Color", mColor), MAKE_NVP("Emission", mEmission), MAKE_NVP("Flags", mFlags),
	   MAKE_NVP("Texture", mTextureHandle));
}

void AstroidComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP("Instances", mNumInstances), MAKE_NVP("Seed", mSeed), MAKE_NVP("Radius", mRadius), MAKE_NVP("Offset", mOffset),
	   MAKE_NVP("Meshes", mMeshHandles), MAKE_NVP("Color", mColor), MAKE_NVP("Emission", mEmission), MAKE_NVP("Flags", mFlags),
	   MAKE_NVP("Texture", mTextureHandle));

	CalculateMatrices();
}

void AstroidComponent::CalculateMatrices()
{
	mMatrices.resize(mNumInstances);

	float radius = mRadius;
	float offset = mOffset;
	srand(mSeed);
	for (uint32_t i = 0; i < mNumInstances; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)i / (float)mNumInstances * 360.f;
		float displacement = GetRandomDisplacement(offset);
		float x = sinf(angle) * radius + displacement;
		displacement = GetRandomDisplacement(offset);
		float y = displacement * 0.4f;
		displacement = GetRandomDisplacement(offset);
		float z = cosf(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		float scale = (rand() % 20) / 100.f + 0.05f;
		model = glm::scale(model, glm::vec3(scale));

		float rot = (float)(rand() % 360);
		model = glm::rotate(model, rot, glm::vec3(0.4f, 0.6f, 0.8f));

		mMatrices[i] = model;
	}
}

REFLECT(AstroidComponent)
{
	BEGIN_REFLECT(AstroidComponent)
	DECLARE_COMPONENT_FUNCS;
}