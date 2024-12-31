#pragma once

#include "core/Core.h"

namespace BHive
{
    class Texture;
    class StaticMesh;
    class Framebuffer;
    class TextureCube;
    class Texture2D;
    class Shader;

    class HDRConverter
    {
    public:
        HDRConverter();

        void SetEnvironmentMap(const Ref<Texture> &texture);
        const Ref<Texture> &GetEnvironmentMap() const { return mEnvironmentTexture; }
        const Ref<Framebuffer> &GetEnvironmentCaptureFBO() const { return mEnvironmentCaptureFBO; }
        const Ref<Framebuffer> &GetIrradianceFBO() const { return mIrradianceFBO; }
        const Ref<Texture> &GetPreFilteredEnvironmentTetxure() const;
        const Ref<Texture> &GetBDRFLUT() const;

        static HDRConverter &Get()
        {
            static HDRConverter converter;
            return converter;
        }

    private:
        void CreateEnvironmentCubeMap();
        void CreateIrradianceMap();
        void CreatePreFilteredEnvironmentMap();
        void CreateBRDFLUTMap();

        Ref<Framebuffer> mEnvironmentCaptureFBO;
        Ref<StaticMesh> mCube;

        Ref<Texture> mEnvironmentTexture;
        Ref<Shader> mEquirectangularShader;
        Ref<Framebuffer> mIrradianceFBO;
        Ref<TextureCube> mPreFilteredEnvironmentTexture;
        Ref<Texture2D> mBRDFLUTTexture;

        Ref<Shader> mIrradianceShader;
        Ref<Shader> mBRDFLUTShader;
        Ref<Shader> mPreFilterEnironmentShader;
    };
}