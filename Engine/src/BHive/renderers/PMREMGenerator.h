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
	class Shader;

	class PMREMGenerator
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
		Ref<Shader> mEquirectangularShader;

		Ref<TextureCube> mPreFilteredEnvironmentTexture;
		Ref<Texture2D> mBRDFLUTTexture;

		Ref<Shader> mIrradianceShader;
		Ref<Shader> mBRDFLUTShader;
		Ref<Shader> mPreFilterEnironmentShader;

		CubeCamera mCubeCamera{.1f, 10.f};

		const std::string GetCommonVertexShader() const;
		const std::string GetEquirectangularShader() const;
		const std::string GetIrradianceShader() const;
		const std::string GetBRDFLUTShader() const;
		const std::string GetPreFilterEnvironmentShader() const;
		const std::string GetCommmonFunctions() const;

		bool mInitialized = false;
	};
} // namespace BHive