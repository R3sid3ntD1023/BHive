#pragma once

#include "core/Core.h"
#include "gfx/cameras/CubeCamera.h"
#include "gfx/textures/RenderTargetCube.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/textures/TextureCube.h"

namespace BHive
{
	class StaticMesh;
	class Framebuffer;
	class Shader;

	class PMREMGenerator
	{
	public:
		PMREMGenerator() = default;

		void Initialize();
		void SetEnvironmentMap(const Ref<Texture> &texture);

		const Ref<Texture> &GetIrradianceTexture() const { return mIrradianceCapture->GetTargetTexture(); }
		const Ref<Texture> &GetPreFilteredEnvironmentTexture() const { return mPreFilteredEnvironmentTexture; }
		const Ref<Texture> &GetBDRFLUT() const { return mBRDFLUTTexture; }

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
		Ref<Shader> mEquirectangularShader;

		Ref<TextureCube> mPreFilteredEnvironmentTexture;
		Ref<Texture2D> mBRDFLUTTexture;

		Ref<Shader> mIrradianceShader;
		Ref<Shader> mBRDFLUTShader;
		Ref<Shader> mPreFilterEnironmentShader;

		CubeCamera mCubeCamera{.1f, 10.f};

		bool mInitialized = false;
	};
} // namespace BHive