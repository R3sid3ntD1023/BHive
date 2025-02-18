#include "MeshComponent.h"
#include "CelestrialBody.h"

void MeshComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP("Color", mColor), MAKE_NVP("Emission", mEmission), MAKE_NVP("Flags", mFlags), MAKE_NVP("Texture", mTextureHandle),
	   MAKE_NVP("Mesh", mMeshHandle));
};

void MeshComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP("Color", mColor), MAKE_NVP("Emission", mEmission), MAKE_NVP("Flags", mFlags), MAKE_NVP("Texture", mTextureHandle),
	   MAKE_NVP("Mesh", mMeshHandle));
};

REFLECT(MeshComponent)
{
	BEGIN_REFLECT(MeshComponent)
	REFLECT_CONSTRUCTOR();
}