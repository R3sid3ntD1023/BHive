#include "Window32.h"
#include <GL/GL.h>
#include "wglext.h"
#include <unordered_map>
#include <windowsx.h>

#pragma comment(lib, "opengl32.lib")

#define CALLWINDOWCALLBACK(func, ...) \
    if (func)                         \
        func(__VA_ARGS__);

namespace BHive
{

    LRESULT WindProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    bool GetWGLFunctions();
    bool InitOpenglContext(HWND hwnd);
    void ShutdownOpengl();

    typedef void *(*WGLPROCADDRESS)(const char *);
    typedef HGLRC (*WGLCREATECONTEXTPROC)(HDC);
    typedef BOOL (*WGLMAKECONTEXTCURRENTPROC)(HDC, HGLRC);
    typedef void (*WGLDELETECONTEXTPROC)(HGLRC);
    typedef BOOL(WINAPI *WGLSWAPINTERVALEXT)(int);
    typedef const char *(*WGLGETGLEXTENSIONSSTRINGEXT)();
    typedef HGLRC (*WGLCREATECONTEXTATTRIBSARB)(HDC, HGLRC, const int *);
    typedef BOOL (*WGLCHOOSEPIXELFORMATARB)(HDC, const int *, const FLOAT *, UINT, int *, UINT *);

    struct WGLContext
    {
        HDC mHDC;
        HGLRC mHandle;
        int interval = 0;
    };

    struct WGLLib
    {
        HMODULE mGLLib;
        WGLPROCADDRESS mWGLProcAddress;
        WGLCREATECONTEXTPROC mCreateContext;
        WGLMAKECONTEXTCURRENTPROC mMakeContextCurrent;
        WGLCREATECONTEXTATTRIBSARB mCreateContextAttribsARB;
        WGLGETGLEXTENSIONSSTRINGEXT mGetGLExtensionsStringEXT;
        WGLSWAPINTERVALEXT mSwapIntervalEXT;
        WGLCHOOSEPIXELFORMATARB mWGLChoosePixelFormatARB;
    };

    struct Win32Lib
    {
        WGLLib mWGL;
        WGLContext mCurrentContext;
        Window32 *mCurrentWindow;

        std::unordered_map<unsigned, EState> mKeyStates;
        EState mMouseButtonStates[7];

        WINDOWCLOSEFUNC mCloseFunc = NULL;
        WINDOWRESIZEFUNC mResizeFunc = NULL;
        WINDOWCHARFUNC mCharFunc = NULL;
        WINDOWKEYFUNC mKeyFunc = NULL;
        WINDOWMOUSEBUTTONFUNC mMouseButtonFunc = NULL;
        WINDOWMOUSEWHEELFUNC mMouseWheelFunc = NULL;
        WINDOWMOUSEMOVEFUNC mMouseMoveFunc = NULL;
    };

    static Win32Lib sWin32Library;

    void *Win32GetProcAddress(const char *proc_name)
    {
        void *p = nullptr;
        if (sWin32Library.mWGL.mWGLProcAddress)
            p = (void *)sWin32Library.mWGL.mWGLProcAddress(proc_name);

        if (p == 0 ||
            (p == (void *)0x1) || (p == (void *)0x2) || (p == (void *)0x3) ||
            (p == (void *)-1))
        {
            p = (void *)GetProcAddress(sWin32Library.mWGL.mGLLib, proc_name);
        }

        return p;
    }

    void MakeContextCurrent(Window32 *window)
    {
        if (!window)
        {
            sWin32Library.mCurrentWindow = nullptr;
            sWin32Library.mWGL.mMakeContextCurrent(NULL, NULL);
            return;
        }

        auto rc = sWin32Library.mCurrentContext.mHandle;
        sWin32Library.mCurrentContext.mHDC = GetDC(window->GetHandle());
        sWin32Library.mCurrentWindow = window;
        sWin32Library.mWGL.mMakeContextCurrent(sWin32Library.mCurrentContext.mHDC, rc);
    }

    Window32 *GetCurrentContext()
    {
        return sWin32Library.mCurrentWindow;
    }

    void SwapBuffers()
    {
        ::SwapBuffers(sWin32Library.mCurrentContext.mHDC);
    }

    bool IsExtensionSupported(const char *extension)
    {
        return strstr(sWin32Library.mWGL.mGetGLExtensionsStringEXT(), extension) != NULL;
    }

    bool IsKeyPressed(unsigned key)
    {
        return (sWin32Library.mKeyStates[key].mAction & Action_Pressed) != 0;
    }

    Window32::Window32(const FWindowProperties &props)
        : mWindowProps(props), mInstance(GetModuleHandle(nullptr))
    {
    }

    Window32::~Window32()
    {
    }

    bool Window32::Init()
    {
        if (!sWin32Library.mCurrentWindow)
            sWin32Library.mCurrentWindow = this;

        if (!GetWGLFunctions())
        {
            return false;
        }

        const char *window_class = "Window32";
        const char *title = mWindowProps.mTitle.c_str();
        const int w = mWindowProps.mWidth;
        const int h = mWindowProps.mHeight;
        const bool vsync = mWindowProps.mVSync;
        sWin32Library.mCurrentContext.interval = vsync ? 1 : 0;

        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.cbClsExtra = NULL;
        wc.cbWndExtra = NULL;
        wc.hInstance = mInstance;
        wc.lpszClassName = window_class;
        wc.lpfnWndProc = WindProc;
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszMenuName = "";
        wc.style = NULL;

        if (!::RegisterClassEx(&wc))
            return false;

        RECT wr = {0, 0, w, h};

        DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        AdjustWindowRectEx(&wr, style, FALSE, WS_EX_APPWINDOW);
        mHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, window_class, title, style, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
                                 NULL, NULL, mInstance, NULL);

        if (!mHandle)
            return false;

        if (!InitOpenglContext(mHandle))
        {
            MessageBox(mHandle, "Failed to init opengl", "Initialization Failed", 0);
        }

        ::ShowWindow(mHandle, SW_SHOW);
        ::UpdateWindow(mHandle);

        return true;
    }

    void Window32::Shutdown()
    {
        if (mHandle)
        {
            ::DestroyWindow(mHandle);
            ::UnregisterClass("Window32", mInstance);
        }
    }

    void Window32::ProcessEvents()
    {
        MSG msg = {};
        while (::PeekMessage(&msg, mHandle, 0, 0, PM_REMOVE) > 0)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    int Window32::GetWidth() const
    {
        return mWindowProps.mWidth;
    }

    int Window32::GetHeight() const
    {
        return mWindowProps.mHeight;
    }

    LRESULT WindProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_CREATE:
        {
            break;
        }
        case WM_SIZE:
        {
            const int width = LOWORD(lparam);
            const int height = HIWORD(lparam);
            const bool inconified = wparam == SIZE_MINIMIZED;
            const bool maximized = wparam == SIZE_MAXIMIZED;

            if (wparam != WS_MINIMIZE)
            {
                CALLWINDOWCALLBACK(sWin32Library.mResizeFunc, width, height);
            }

            sWin32Library.mCurrentWindow->mWindowProps.mWidth = width;
            sWin32Library.mCurrentWindow->mWindowProps.mHeight = height;

            return 0;
        }
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            WORD vkCode = LOWORD(wparam);
            WORD scanCode = LOBYTE(lparam);
            WORD flags = HIWORD(lparam);
            BOOL key_is_down = (flags & KF_REPEAT) == KF_REPEAT;

            switch (vkCode)
            {
            case VK_SHIFT:
            case VK_CONTROL:
            case VK_MENU: // alt
            {
                vkCode = LOWORD(MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX));
                break;
            }
            }

            EState &state = sWin32Library.mKeyStates[vkCode];
            state.mAction = (flags & KF_UP) == KF_UP ? Action_Released : Action_Pressed;

            if (key_is_down)
            {
                state.mRepeatCount += LOWORD(lparam);
                state.mAction |= Action_Repeat;
            }
            else
            {
                state.mRepeatCount = LOWORD(lparam);
            }

            CALLWINDOWCALLBACK(sWin32Library.mKeyFunc, vkCode, state);
            break;
        }
        case WM_CHAR:
        {
            CALLWINDOWCALLBACK(sWin32Library.mCharFunc, wparam);
            return 0;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDOWN:
        {
            int button;
            EState action;

            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
                button = 0;
            else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
                button = 1;
            else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
                button = 2;
            else if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
                button = 3;
            else
                button = 4;

            if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_XBUTTONDOWN ||
                msg == WM_MBUTTONDOWN)
            {
                action.mAction = Action_Pressed;
            }
            else
            {
                action.mAction = Action_Released;
            }

            sWin32Library.mMouseButtonStates[button] = action;
            CALLWINDOWCALLBACK(sWin32Library.mMouseButtonFunc, button, action);

            if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP)
                return TRUE;

            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            CALLWINDOWCALLBACK(sWin32Library.mMouseWheelFunc, 0.0, GET_WHEEL_DELTA_WPARAM(wparam) / (double)WHEEL_DELTA);
            return 0;
        }
        case WM_MOUSEHWHEEL:
        {
            CALLWINDOWCALLBACK(sWin32Library.mMouseWheelFunc, -GET_WHEEL_DELTA_WPARAM(wparam) / (double)WHEEL_DELTA, 0.0);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            const int x = GET_X_LPARAM(lparam);
            const int y = GET_Y_LPARAM(lparam);

            CALLWINDOWCALLBACK(sWin32Library.mMouseMoveFunc, x, y);

            return 0;
        }
        case WM_CLOSE:
        {
            CALLWINDOWCALLBACK(sWin32Library.mCloseFunc);

            ShutdownOpengl();
            break;
        }
        case WM_DESTROY:
        {
            ::PostQuitMessage(0);
            return 0;
        }
        }

        return ::DefWindowProc(hwnd, msg, wparam, lparam);
    }

    bool GetWGLFunctions()
    {

        HWND dummy = CreateWindowEx(0, "STATIC", "DummyWindow",
                                    WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                    NULL, NULL, NULL, NULL);
        ASSERT(dummy, "Failed to create dummy window!");

        HDC dummyDC = GetDC(dummy);

        PIXELFORMATDESCRIPTOR pfd = {0};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cAlphaBits = 8;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        const int pf = ::ChoosePixelFormat(dummyDC, &pfd);
        if (pf == 0)
            return false;

        int format = ChoosePixelFormat(dummyDC, &pfd);
        ASSERT(format, "Cannot choose Opengl format for window!");

        if (::SetPixelFormat(dummyDC, pf, &pfd) == FALSE)
            return false;

        if (!sWin32Library.mWGL.mGLLib)
            sWin32Library.mWGL.mGLLib = LoadLibrary("opengl32.dll");

        if (!sWin32Library.mWGL.mGLLib)
            return false;

        if (!sWin32Library.mWGL.mWGLProcAddress)
            sWin32Library.mWGL.mWGLProcAddress = (WGLPROCADDRESS)Win32GetProcAddress("wglGetProcAddress");

        auto tempRC = wglCreateContext(dummyDC);
        ASSERT(tempRC, "Failed to create opengl context for dummy Window!");

        int ok = wglMakeCurrent(dummyDC, tempRC);
        ASSERT(ok, "Failed to make current opengl context for dummy window");

        auto wgl_address = sWin32Library.mWGL.mWGLProcAddress;
        sWin32Library.mWGL.mCreateContext = (WGLCREATECONTEXTPROC)Win32GetProcAddress("wglCreateContext");
        sWin32Library.mWGL.mMakeContextCurrent = (WGLMAKECONTEXTCURRENTPROC)Win32GetProcAddress("wglMakeCurrent");
        sWin32Library.mWGL.mCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARB)Win32GetProcAddress("wglCreateContextAttribsARB");
        sWin32Library.mWGL.mWGLChoosePixelFormatARB = (WGLCHOOSEPIXELFORMATARB)Win32GetProcAddress("wglChoosePixelFormatARB");
        sWin32Library.mWGL.mGetGLExtensionsStringEXT = (WGLGETGLEXTENSIONSSTRINGEXT)Win32GetProcAddress("wglGetExtensionsStringEXT");

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempRC);
        ReleaseDC(dummy, dummyDC);
        DestroyWindow(dummy);
    }

    bool InitOpenglContext(HWND hwnd)
    {
        sWin32Library.mCurrentContext.mHDC = GetDC(hwnd);
        ASSERT(sWin32Library.mCurrentContext.mHDC, "Cannot get device context for window!");

        if (!IsExtensionSupported("WGL_ARB_create_context_profile"))
        {
            return false;
        }

        const int pixelAttribs[] =
            {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                // WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                //  WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
                WGL_COLOR_BITS_ARB, 32,
                WGL_ALPHA_BITS_ARB, 8,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                // WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
                // WGL_SAMPLE_BUFFERS_ARB, 1,
                // WGL_SAMPLES_ARB, 4,
                0

            };

        const int attribs[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB,
                4,
                WGL_CONTEXT_MINOR_VERSION_ARB,
                6,

                WGL_CONTEXT_PROFILE_MASK_ARB,
                WGL_CONTEXT_CORE_PROFILE_BIT_ARB,

#ifdef _DEBUG
                WGL_CONTEXT_FLAGS_ARB,
                WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
                0};

        if (!sWin32Library.mCurrentContext.mHandle)
        {
            int format;
            UINT formats;
            if (!sWin32Library.mWGL.mWGLChoosePixelFormatARB(sWin32Library.mCurrentContext.mHDC, pixelAttribs, NULL, 1, &format, &formats) || formats == 0)
            {
                LOG_ERROR("Opengl does not support pixel format!");
                return false;
            }

            PIXELFORMATDESCRIPTOR desc = {.nSize = sizeof(desc)};
            int ok = ::DescribePixelFormat(sWin32Library.mCurrentContext.mHDC, format, sizeof(desc), &desc);
            ASSERT(ok, "Failed to describe Opengl pixel format");

            if (!::SetPixelFormat(sWin32Library.mCurrentContext.mHDC, format, &desc))
            {
                LOG_ERROR("Cannot Set Opengl selected pixel format!");
                return false;
            }

            sWin32Library.mCurrentContext.mHandle = sWin32Library.mWGL.mCreateContextAttribsARB(sWin32Library.mCurrentContext.mHDC, NULL, attribs);
            wglMakeCurrent(sWin32Library.mCurrentContext.mHDC, sWin32Library.mCurrentContext.mHandle);

            // MessageBoxA(0, (char *)glGetString(GL_VERSION), "OPENGL VERSION", 0);
        }

        if (IsExtensionSupported("WGL_EXT_swap_control"))
        {
            sWin32Library.mWGL.mSwapIntervalEXT = (WGLSWAPINTERVALEXT)Win32GetProcAddress("wglSwapIntervalEXT");
        }

        if (sWin32Library.mWGL.mSwapIntervalEXT)
        {
            sWin32Library.mWGL.mSwapIntervalEXT(sWin32Library.mCurrentContext.interval);
        }

        wglMakeCurrent(NULL, NULL);

        return true;
    }

    void ShutdownOpengl()
    {
        wglDeleteContext(sWin32Library.mCurrentContext.mHandle);
    }

    void SetWindowCloseFunc(WINDOWCLOSEFUNC closefun)
    {
        sWin32Library.mCloseFunc = closefun;
    }

    void SetWindowResizeFunc(WINDOWRESIZEFUNC resizefun)
    {
        sWin32Library.mResizeFunc = resizefun;
    }

    void SetWindowCharFunc(WINDOWCHARFUNC charfun)
    {
        sWin32Library.mCharFunc = charfun;
    }

    void SetWindowKeyFunc(WINDOWKEYFUNC keyfun)
    {
        sWin32Library.mKeyFunc = keyfun;
    }

    void SetWindowMouseButtonFunc(WINDOWMOUSEBUTTONFUNC func)
    {
        sWin32Library.mMouseButtonFunc = func;
    }

    void SetWindowMouseWheelFunc(WINDOWMOUSEWHEELFUNC func)
    {
        sWin32Library.mMouseWheelFunc = func;
    }

    void SetWindowMouseMoveFunc(WINDOWMOUSEMOVEFUNC func)
    {
        sWin32Library.mMouseMoveFunc = func;
    }

} // namespace BHive
