#pragma once

#include <core/Core.h>
#include <gfx/cameras/EditorCamera.h>
#include <core/Layer.h>
#include <core/events/ApplicationEvents.h>

BEGIN_NAMESPACE(BHive)

class World;
class ResourceManager;
class SceneRenderer;

struct SolarSystemLayer : public Layer
{
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;

	virtual void OnEvent(Event &e);

	virtual void OnGuiRender() override;

private:
	bool OnWindowResize(WindowResizeEvent &e);

private:
	EditorCamera mCamera;

	Ref<World> mWorld;
	Ref<ResourceManager> mResourceManager;
	Ref<SceneRenderer> mRenderer;
};

END_NAMESPACE