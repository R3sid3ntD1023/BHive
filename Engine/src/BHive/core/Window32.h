#pragma once

#include "core/Core.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace BHive
{
    enum EAction : unsigned char
    {
        Action_Released,
        Action_Pressed,
        Action_Repeat
    };
    enum EMod : unsigned char
    {
        Mod_None = 0,
        Mod_ALT = 1 << 0,
        Mod_CTRL = 1 << 1,
        Mod_SHIFT = 1 << 2
    };

    struct EState
    {
        unsigned mAction = 0;
        unsigned mMods = 0;
        unsigned mRepeatCount = 0;
    };

    typedef void (*LOADPROC)();
    typedef void (*WINDOWCLOSEFUNC)();
    typedef void (*WINDOWRESIZEFUNC)(int, int);
    typedef void (*WINDOWCHARFUNC)(unsigned);
    typedef void (*WINDOWKEYFUNC)(unsigned, EState);
    typedef void (*WINDOWMOUSEBUTTONFUNC)(int, EState);
    typedef void (*WINDOWMOUSEWHEELFUNC)(double, double);
    typedef void (*WINDOWMOUSEMOVEFUNC)(int, int);

    struct FWindowProperties
    {
        std::string mTitle = "Window";
        int mWidth = 800, mHeight = 600;
        bool mVSync = false;
    };

    class Window32
    {
    private:
        /* data */
    public:
        Window32(const FWindowProperties &props = {});
        ~Window32();

        bool Init();
        void Shutdown();
        void ProcessEvents();

        int GetWidth() const;
        int GetHeight() const;

        HWND GetHandle() const { return mHandle; }

    private:
        HWND mHandle;
        HINSTANCE mInstance;
        FWindowProperties mWindowProps;
        friend LRESULT WindProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    };

    void SetWindowCloseFunc(WINDOWCLOSEFUNC func);
    void SetWindowResizeFunc(WINDOWRESIZEFUNC func);
    void SetWindowCharFunc(WINDOWCHARFUNC func);
    void SetWindowKeyFunc(WINDOWKEYFUNC func);
    void SetWindowMouseButtonFunc(WINDOWMOUSEBUTTONFUNC func);
    void SetWindowMouseWheelFunc(WINDOWMOUSEWHEELFUNC func);
    void SetWindowMouseMoveFunc(WINDOWMOUSEMOVEFUNC func);

    void *Win32GetProcAddress(const char *proc_name);

    void MakeContextCurrent(Window32 *window);

    Window32 *GetCurrentContext();

    void SwapBuffers();

    bool IsExtensionSupported(const char *extension);

    bool IsKeyPressed(unsigned key);

} // namespace BHive
