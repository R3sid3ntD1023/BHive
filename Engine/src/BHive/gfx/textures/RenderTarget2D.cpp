#include "RenderTarget2D.h"
#include "gfx/TextureUtils.h"
#include <glad/glad.h>
#include "renderers/Renderer.h"
#include "math/Frustum.h"

namespace BHive
{
	struct CameraDirection
	{
		GLenum CubemapFace;
		glm::vec3 Target;
		glm::vec3 Up;
	};

	CameraDirection sCameraDirections[] = {
		{GL_TEXTURE_CUBE_MAP_POSITIVE_X, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{GL_TEXTURE_CUBE_MAP_NEGATIVE_X, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{GL_TEXTURE_CUBE_MAP_POSITIVE_Y, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{GL_TEXTURE_CUBE_MAP_POSITIVE_Z, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}}};

	RenderTarget2D::RenderTarget2D(uint32_t size, float radius)
		: mSize(size),
		  mRadius(radius)
	{
		mTargetTexture = CreateRef<TextureCube>(
			size, FTextureSpecification{.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		glCreateFramebuffers(1, &mFramebufferID);
		glNamedFramebufferDrawBuffer(mFramebufferID, GL_COLOR_ATTACHMENT0);

		glCreateRenderbuffers(1, &mRenderBufferID);
		glNamedRenderbufferStorage(mRenderBufferID, GL_DEPTH_COMPONENT24, size, size);
		glNamedFramebufferRenderbuffer(mFramebufferID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferID);

		ASSERT(glCheckNamedFramebufferStatus(mFramebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	RenderTarget2D::~RenderTarget2D()
	{
		glDeleteFramebuffers(1, &mFramebufferID);
		glDeleteRenderbuffers(1, &mRenderBufferID);
	}

	void RenderTarget2D::Bind(uint32_t face)
	{
		auto proj = glm::perspective(90.0f, 1.0f, 1.0f, mRadius);
		auto view = glm::lookAt({}, sCameraDirections[face].Target, sCameraDirections[face].Up);

		Renderer::SubmitCamera(proj, view);
		FrustumViewer frustum(proj, view);
		LineRenderer::DrawFrustum(frustum, 0xff00ffff);

		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
		glNamedFramebufferTextureLayer(mFramebufferID, GL_COLOR_ATTACHMENT0, mTargetTexture->GetRendererID(), 0, face);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, mSize, mSize);
	}

	void RenderTarget2D::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

} // namespace BHive