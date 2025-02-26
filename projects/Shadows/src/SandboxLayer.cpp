#include "SandboxLayer.h"
#include "renderers/Renderer.h"
#include "core/Application.h"
#include "gfx/RenderCommand.h"
#include "mesh/primitives/Plane.h"
#include "gfx/Shader.h"
#include "mesh/indirect_mesh/IndirectMesh.h"
#include "mesh/MeshImporter.h"

namespace BHive
{
#define SSBO_INDEX_PER_OBJECT_DATA 1

	struct PerObjectData
	{
		glm::mat4 WorldMatrix;
	};

	struct FDrawElementsIndirectCommand
	{
		uint32_t count;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t baseVertex;
		uint32_t baseInstance;
	};

	void SandboxLayer::OnAttach()
	{
		auto &window = Application::Get().GetWindow();
		auto windowSize = window.GetSize();
		auto aspect = windowSize.x / windowSize.y;

		mCamera = EditorCamera(45.0f, aspect, .01f, 1000.f);

		RenderCommand::ClearColor(.3f, .3f, .3f, 1.0f);

		mPlane = CreateRef<PPlane>(5.f, 5.f);
		mIndirectPlane = CreateRef<IndirectRenderable>();
		mIndirectPlane->Init(mPlane, 2);

		FMeshImportData data;
		if (MeshImporter::Import(RESOURCE_PATH "industrial_standing_light/scene.gltf", data))
		{
			mLightPost = CreateRef<StaticMesh>(data.mMeshData);
			mIndirectLightPost = CreateRef<IndirectRenderable>();
			mIndirectLightPost->Init(mLightPost, 2);
		}

		mShader = ShaderLibrary::Load(RESOURCE_PATH "ShadowShader.glsl");
	}

	void SandboxLayer::OnUpdate(float dt)
	{
		mCamera.ProcessInput();

		RenderCommand::Clear();

		Renderer::Begin(mCamera.GetProjection(), mCamera.GetView().inverse());

		LineRenderer::DrawGrid({});

		mShader->Bind();
		mShader->SetUniform("uColor", glm::vec3(.5, .5, .5));

		glm::mat4 matrices[] = {FTransform({-10, 0, 0}), FTransform({10, 0, 0})};
		mIndirectPlane->Draw(FTransform({0, 0, 0}, {-90, 0, 0}), matrices);

		mShader->SetUniform("uColor", glm::vec3(1, .5f, 0));
		mIndirectLightPost->Draw(FTransform({}, {0, 0, 0}), matrices);

		mShader->UnBind();

		Renderer::End();
	}

	void SandboxLayer::OnDetach()
	{
	}

	void SandboxLayer::OnEvent(Event &e)
	{
		mCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &SandboxLayer::OnWindowResize);
	}

	bool SandboxLayer::OnWindowResize(WindowResizeEvent &e)
	{
		mCamera.Resize(e.x, e.y);

		RenderCommand::SetViewport(0, 0, e.x, e.y);

		return false;
	}

} // namespace BHive