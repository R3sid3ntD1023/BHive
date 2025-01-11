#include <core/Window32.h>
#include <glad/glad.h>
#include <core/Log.h>
#include <events/KeyCodes.h>
#include "future_test.h"
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/VertexArray.h>
#include "shader.embedded"
#include "threading/Threading.h"
#include "core/Time.h"
#include "renderers/Renderer.h"
#include "math/Math.h"

using namespace BHive;

bool is_running = true;

Ref<Shader> sShader;
Ref<Texture> sTexture;
Ref<VertexArray> sVAO;

void Render()
{
    auto dt = Time::GetDeltaTime();
    Thread::Update();

    RenderCommand::ClearColor(.1f, .1f, .1f, 1);
    RenderCommand::Clear(Buffer_Color | Buffer_Depth);

    Renderer::Begin(glm::perspective(45.f, 1.0f, 0.1f, 1000.f), glm::lookAt(glm::vec3{0, 4, 2}, glm::vec3{0, 0, 0}, {0, 1, 0}));

    LineRenderer::DrawBox({1, 1, 1}, {0, 1, 0}, 0xFF00FFFF);

    sShader->Bind();
    sTexture->Bind();

    RenderCommand::DrawElements(EDrawMode::Triangles, *sVAO);

    Renderer::End();

    // sShader->UnBind();

    BHive::SwapBuffers();
}

void OnWindowResize(int w, int h)
{
    RenderCommand::SetViewport(0, 0, w, h);

    Render();
}

void OnChar(unsigned c)
{
    LOG_TRACE("{}", (char)c);
};

void OnKey(unsigned key, EState state)
{
    LOG_TRACE("{}, {}, {}, {}", key, state.mAction, state.mMods, state.mRepeatCount);

    switch (key)
    {
    case BHive::Key::Space:
    {
        LOG_TRACE("Jump");
    }
    break;

    default:
        break;
    }
}

void OnMouse(int button, EState state)
{
    LOG_TRACE("{}, {}", button, state.mAction);
}

void OnMouseWheel(double x, double y)
{
    LOG_TRACE("{}, {}", x, y);
}

void OnMouseMoved(int x, int y)
{
    LOG_TRACE("{}, {}", x, y);
}

int main(int argc, char **argv)
{
    BHive::Log::Init();

    BHive::FWindowProperties props = {};
    BHive::Window32 *window = new BHive::Window32(props);
    window->Init();

    SetWindowCloseFunc([]()
                       { is_running = false; });
    SetWindowResizeFunc(OnWindowResize);
    SetWindowCharFunc(OnChar);
    SetWindowKeyFunc(OnKey);
    SetWindowMouseButtonFunc(OnMouse);
    SetWindowMouseWheelFunc(OnMouseWheel);
    SetWindowMouseMoveFunc(OnMouseMoved);

    BHive::MakeContextCurrent(window);
    int status = gladLoadGLLoader((GLADloadproc)BHive::Win32GetProcAddress);
    if (!status)
        return 0;

    Renderer::Init();
    RenderCommand::Init();

    sTexture = TestFuture();
    sShader = Shader::Create("plane", s_vertex, s_fragment);

    float vertices[] = {
        -.5f, -0.5f, 0.0f, 0, 0,
        .5f, -.5f, 0, 1, 0,
        .5f, .5f, 0, 1, 1,
        -.5f, .5f, 0, 0, 1};

    uint32_t indices[] = {0, 1, 2, 2, 3, 0};

    auto vb = VertexBuffer::Create(vertices, 20 * sizeof(float));
    vb->SetLayout({{EShaderDataType::Float3}, {EShaderDataType::Float2}});
    auto ib = IndexBuffer::Create(indices, 6);
    sVAO = VertexArray::Create();
    sVAO->AddVertexBuffer(vb);
    sVAO->SetIndexBuffer(ib);

    while (is_running)
    {
        window->ProcessEvents();

        Render();
    }

    window->Shutdown();

    delete window;

    return 0;
}