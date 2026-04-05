#pragma once

#include "core/Core.h"
#include "gfx/cameras/CubeCamera.h"
#include "gfx/textures/RenderTargetCube.h"

namespace BHive
{
	class Texture;
	class StaticMesh;
	class Framebuffer;
	class TextureCube;
	class Texture2D;
	class Material;

	class BHIVE_API PMREMGenerator
	{
	public:
		PMREMGenerator() = default;

		void Initialize();
		void SetEnvironmentMap(const Ref<Texture> &texture);

		const Ref<Texture> &GetIrradianceTexture() const;
		const Ref<Texture> &GetPreFilteredEnvironmentTetxure() const;
		const Ref<Texture> &GetBDRFLUT() const;

	private:
		void CreateEnvironmentCubeMap();
		void CreateIrradianceMap();
		void CreatePreFilteredEnvironmentMap();
		void CreateBRDFLUTMap();

		Ref<Texture> mEnvironment;
		Ref<TextureCube> mEnvironmentCube;
		Ref<TextureCube> mPreFilteredEnvironment;
		Ref<TextureCube> mIrradiance;
		Ref<Texture2D> mBRDFLUT;

		Ref<Material> mEquirectangularMat;
		Ref<Material> mIrradianceMat;
		Ref<Material> mBRDFLUTMat;
		Ref<Material> mPreFilterEnironmentMat;

		bool mInitialized = false;
	};
} // namespace BHive