#include "ShaderProgram.h"

namespace BHive
{
	ShaderProgram::ShaderProgram(const Ref<ShaderAsset> &asset)
		: mAsset(asset)
	{
		mMerged = FShaderReflection::Merge(asset->Reflection);
	}
}