#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dwmapi.h>
#include "../../ext/imgui/imgui.h"
#include "../../ext/imgui/imgui_impl_win32.h"
#include "../../ext/imgui/imgui_impl_dx11.h"
#include "../../src/core/variables/variables.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

class OverlayWindow {
private:
    HWND windowHandle;
    WNDCLASSEXW windowClass;

    ID3D11Device* d3dDevice;
    ID3D11DeviceContext* d3dContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTarget;

    void SetupD3D11(HWND hwnd) {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;

        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
        D3D_FEATURE_LEVEL obtainedLevel;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            levels, 2, D3D11_SDK_VERSION, &sd,
            &swapChain, &d3dDevice, &obtainedLevel, &d3dContext
        );

        if (hr == DXGI_ERROR_UNSUPPORTED) {
            D3D11CreateDeviceAndSwapChain(
                nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
                levels, 2, D3D11_SDK_VERSION, &sd,
                &swapChain, &d3dDevice, &obtainedLevel, &d3dContext
            );
        }

        ID3D11Texture2D* backBuffer = nullptr;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (backBuffer) {
            d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTarget);
            backBuffer->Release();
        }
    }

    void CleanupD3D11() {
        if (renderTarget) { renderTarget->Release(); renderTarget = nullptr; }
        if (swapChain) { swapChain->Release(); swapChain = nullptr; }

        if (d3dContext) { d3dContext->Release(); d3dContext = nullptr; }
        if (d3dDevice) { d3dDevice->Release(); d3dDevice = nullptr; }
    }

public:
    OverlayWindow() : windowHandle(nullptr), d3dDevice(nullptr), d3dContext(nullptr),
        swapChain(nullptr), renderTarget(nullptr) {
        ZeroMemory(&windowClass, sizeof(windowClass));
    }

    bool Initialize() {
        windowClass.cbSize = sizeof(WNDCLASSEXW);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = OverlayWndProc;
        windowClass.hInstance = GetModuleHandleW(nullptr);

        windowClass.lpszClassName = L"Roblox External";

        if (!RegisterClassExW(&windowClass))
            return false;

        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);

        windowHandle = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
            windowClass.lpszClassName, L"Roblox External",
            WS_POPUP, 0, 0, screenW, screenH,
            nullptr, nullptr, windowClass.hInstance, nullptr
        );

        if (!windowHandle)
            return false;

        SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), 255, LWA_ALPHA);

        MARGINS margins = { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(windowHandle, &margins);

        ShowWindow(windowHandle, SW_SHOW);
        UpdateWindow(windowHandle);

        SetupD3D11(windowHandle);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;

        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(windowHandle);
        ImGui_ImplDX11_Init(d3dDevice, d3dContext);

        return true;
    }

    void BeginFrame() {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (variables::menuOpen) {
            SetWindowLong(windowHandle, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        }
        else {
            SetWindowLong(windowHandle, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void RenderMenu() {
        if (!variables::menuOpen) return;

        ImGui::SetNextWindowSize(ImVec2(650, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Roblox External", &variables::menuOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImGui::BeginChild("TabBar", ImVec2(160, 0), true);

        ImGui::SetCursorPosX(15);
        ImVec2 buttonSize(130, 40);

        if (variables::selectedTab == 0) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
            if (ImGui::Button("Aimbot", buttonSize)) variables::selectedTab = 0;
            ImGui::PopStyleColor(2);
        }
        else {
            if (ImGui::Button("Aimbot", buttonSize)) variables::selectedTab = 0;
        }

        ImGui::SetCursorPosX(15);
        if (variables::selectedTab == 1) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
            if (ImGui::Button("Visuals", buttonSize)) variables::selectedTab = 1;
            ImGui::PopStyleColor(2);
        }
        else {
            if (ImGui::Button("Visuals", buttonSize)) variables::selectedTab = 1;
        }

        ImGui::SetCursorPosX(15);
        if (variables::selectedTab == 2) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
            if (ImGui::Button("Local", buttonSize)) variables::selectedTab = 2;
            ImGui::PopStyleColor(2);
        }
        else {
            if (ImGui::Button("Local", buttonSize)) variables::selectedTab = 2;
        }

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("Content", ImVec2(0, 0), true);

        if (variables::selectedTab == 0) {
            ImGui::Text("Soon..");
        }
        else if (variables::selectedTab == 1) {
            ImGui::Text("Visuals");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Enable ESP", &variables::ESP::enabled);
            ImGui::Spacing();
            ImGui::Checkbox("Boxes", &variables::ESP::boxes);
            ImGui::Checkbox("Names", &variables::ESP::names);

            ImGui::Checkbox("Distance", &variables::ESP::distance);
            ImGui::Checkbox("Health Bar", &variables::ESP::healthBar);
        }
        else if (variables::selectedTab == 2) {
            ImGui::Text("Local");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("WalkSpeed", &variables::Local::speedEnabled);
            ImGui::SliderFloat("##WalkSpeed", &variables::Local::walkSpeed, 16.0f, 200.0f, "%.0f");

            ImGui::Spacing();
            ImGui::Checkbox("JumpPower", &variables::Local::jumpEnabled);
            ImGui::SliderFloat("##JumpPower", &variables::Local::jumpPower, 50.0f, 200.0f, "%.0f");
        }

        ImGui::EndChild();

        ImGui::End();
    }

    void EndFrame() {
        ImGui::Render();

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        d3dContext->OMSetRenderTargets(1, &renderTarget, nullptr);
        d3dContext->ClearRenderTargetView(renderTarget, clearColor);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swapChain->Present(0, 0);
    }

    void Cleanup() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupD3D11();

        if (windowHandle) {
            DestroyWindow(windowHandle);
            windowHandle = nullptr;
        }

        UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
    }

    HWND GetWindowHandle() const { return windowHandle; }
};
