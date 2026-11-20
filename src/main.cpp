#include <windows.h>
#include <d3d11.h>
#include <iostream>
#include <vulkan/vulkan.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "user32.lib")

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
            break;
        }

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_SIZE:
        break;

    default:
    return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool InitializeD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        nullptr,
        &g_pd3dDeviceContext
    );

    return SUCCEEDED(hr);
}

const char* getBuild() {
#if defined(__x86_64__) || defined(_M_X64)
    return "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
    return "x86_32";
#elif defined(__aarch64__) || defined(_M_ARM64)
    return "ARM64";
#elif defined(_M_ARM) || defined(__arm__)
    return "ARM32";
#else
    return "UNKNOWN";
#endif
}

const char* getPlatform() {
#if defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__) && defined(__MACH__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "UNKNOWN";
#endif
}

void logInfo(const char* message) {
    std::cout << "Info: " << message << std::endl;
}

void   logError(const char* message) {
    std::cerr << "Error: " << message << std::endl;
}

void logWarning(const char* message) {
    std::cerr << "Warning: " << message << std::endl;
}

void logDebug(const char* message) {
    std::cout << "Debug: " << message << std::endl;
} 

void CleanupD3D() {
    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

int main() {
    std::cout << "PS5Bridge" << std::endl;
    std::cout << "Version: 0.1" << std::endl;
    std::cout << "Build: " << getBuild() << std::endl;
    std::cout << "Platform: " << getPlatform() << std::endl;
    std ::cout << "PS5Bridge is an under development project which allows you to run PlayStation 5 games on your PC." << std::endl;
    logInfo("sample logInfo message");
    logError("sample logError message");
    logWarning("sample logWarning message");
    logDebug("sample logDebug message");

#ifdef _WIN32 
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"Custom";
    
ATOM atom = RegisterClassExW(&wc);

if (atom == 0) {
    logError("Failed to register window class");
    DWORD error = GetLastError();
    std::cerr << "Error code: " << error << std::endl;
    return -1;
}
else {
    logInfo("Window class registered successfully");
    std::cout << "Window class atom: " << atom << std::endl;
}

    HWND hwnd = CreateWindowExW(0, L"Custom",
         L"D3D Window",
          WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT,
            CW_USEDEFAULT,
            800,
            600,
            nullptr,
            nullptr,
            nullptr,
            nullptr);

    if (!hwnd) {
        logError("Failed to create window");
        return -1;
    }

    MSG msg = {};
    if (!InitializeD3D(hwnd)) {
        logError("Failed to initialize D3D");
        CleanupD3D();
        return -1;
    }
    else {
        logInfo("D3D initialized successfully");
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);
    }

    bool running = true;
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    if (!running) {
        logInfo("Exiting application");
        CleanupD3D();
    }
#else 
        logInfo("D3D initialized successfully");
#endif
    return 0;
}