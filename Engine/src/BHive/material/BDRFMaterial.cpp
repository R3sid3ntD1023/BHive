#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "BDRFMaterial.h"
#include "renderers/Renderer.h"

namespace BHive
{
	BDRFMaterial::BDRFMaterial()
	{
		
	}

	void BDRFMaterial::Submit() const
	{
	}


	Ref<Shader> BDRFMaterial::GetShader() const
	{
		static Ref<Shader> shader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/BDRFBDRFMaterial.glsl");
		return shader;
	}

	void BDRFMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(mAldebo, mMetallic, mRoughness, mDiaElectric, mEmission, mOpacity, mTiling, mDepthScale, mFlags);
	}

	void BDRFMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(mAldebo, mMetallic, mRoughness, mDiaElectric, mEmission, mOpacity, mTiling, mDepthScale, mFlags);
	}

	bool BDRFMaterial::CastShadows() const
	{
		return (mFlags & MaterialFlag_Cast_Shadows) != 0;
	}

	bool BDRFMaterial::IsTransparent() const
	{
		return (mFlags & MaterialFlag_Transparent) != 0 || mOpacity < 1.0f ||
			   (mFlags & MaterialFlag_Alpha_Is_Transparency) != 0;
	}
} // namespace BHive