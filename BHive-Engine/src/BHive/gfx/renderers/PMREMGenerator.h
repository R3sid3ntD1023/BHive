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
		void RenderCube(uint32_t face);

		Ref<RenderTargetCube> mEnvironmentCapture;
		Ref<RenderTargetCube> mIrradianceCapture;

		Ref<StaticMesh> mCube;

		Ref<Texture> mEnvironmentTexture;
		Ref<TextureCube> mPreFilteredEnvironmentTexture;
		Ref<Texture2D> mBRDFLUTTexture;

		Ref<Material> mEquirectangularMat;
		Ref<Material> mIrradianceMat;
		Ref<Material> mBRDFLUTMat;
		Ref<Material> mPreFilterEnironmentMat;

		CubeCamera mCubeCamera{.1f, 10.f};

		bool mInitialized = false;
	};
} // namespace BHive