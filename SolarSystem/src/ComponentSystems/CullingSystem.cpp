#include "CelestrialBody.h"
#include "components/AstroidComponent.h"
#include "components/IDComponent.h"
#include "components/MeshComponent.h"
#include "CullingSystem.h"
#include "Universe.h"
#include <asset/AssetManager.h>
#include <gfx/RenderCommand.h>
#include <renderers/Renderer.h>
#include <gfx/Shader.h>
#include <mesh/StaticMesh.h>
#include <gfx/StorageBuffer.h>
#include <glad/glad.h>
#include <core/profiler/CPUGPUProfiler.h>

CullingSystem::CullingSystem()
{
	mCullingShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "Shaders/CullingShader.glsl");
}

void CullingSystem::Update(Universe *universe, float dt)
{
}