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
            ImGui::Text("Aimbot");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Enable Aimbot", &variables::Aimbot::enabled);
            ImGui::Checkbox("Show FOV", &variables::Aimbot::showFOV);

            ImGui::Spacing();
            ImGui::Text("FOV Radius");
            ImGui::SliderFloat("##FOV", &variables::Aimbot::fovRadius, 10.0f, 500.0f, "%.0f");

            ImGui::Text("Smoothing");
            ImGui::SliderFloat("##Smoothing", &variables::Aimbot::smoothing, 1.0f, 20.0f, "%.1f");

            ImGui::Spacing();
            ImGui::Text("Aim Target");
            const char* targets[] = { "Head", "HumanoidRootPart" };
            ImGui::Combo("##Target", &variables::Aimbot::aimTarget, targets, 2);

            ImGui::Spacing();
            ImGui::Text("Aimbot Keybind");
            const char* keys[] = { "None", "Left Mouse", "Right Mouse", "Middle Mouse", "X1 Mouse", "X2 Mouse",
                                   "Shift", "Ctrl", "Alt", "C", "V", "X", "Z", "Q", "E", "R", "T", "F", "G" };
            const int keyValues[] = { 0, 1, 2, 4, 5, 6, 16, 17, 18, 0x43, 0x56, 0x58, 0x5A, 0x51, 0x45, 0x52, 0x54, 0x46, 0x47 };

            int currentKeyIndex = 0;
            for (int i = 0; i < 19; i++) {
                if (keyValues[i] == variables::Aimbot::aimbotKey) {
                    currentKeyIndex = i;
                    break;
                }
            }

            if (ImGui::Combo("##Keybind", &currentKeyIndex, keys, 19)) {
                variables::Aimbot::aimbotKey = keyValues[currentKeyIndex];
            }
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
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Checkbox("Snaplines", &variables::ESP::snaplines);
            
            if (variables::ESP::snaplines) {
                ImGui::Spacing();
                ImGui::Text("Origin");
                const char* origins[] = { "Cursor", "Center", "Top", "Bottom", "Local Head", "Local HRP" };
                ImGui::Combo("##SnaplinesOrigin", &variables::ESP::snaplinesOrigin, origins, 6);
                
                ImGui::Text("Destination");
                const char* destinations[] = { "Head", "HumanoidRootPart", "Closest Part" };
                ImGui::Combo("##SnaplinesDestination", &variables::ESP::snaplinesDestination, destinations, 3);
                
                ImGui::Text("Style");
                const char* styles[] = { "Straight", "Curved", "Dashed" };
                ImGui::Combo("##SnaplinesStyle", &variables::ESP::snaplinesStyle, styles, 3);
                
                ImGui::Text("Thickness");
                ImGui::SliderFloat("##SnaplinesThickness", &variables::ESP::snaplinesThickness, 0.5f, 5.0f, "%.1f");
                
                ImGui::Checkbox("Outline", &variables::ESP::snaplinesOutline);
            }
        }
        else if (variables::selectedTab == 2) {
            ImGui::Text("Exploits");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("WalkSpeed", &variables::Local::speedEnabled);
            ImGui::SliderFloat("##WalkSpeed", &variables::Local::walkSpeed, 16.0f, 200.0f, "%.0f");

            ImGui::Spacing();
            ImGui::Checkbox("JumpPower", &variables::Local::jumpEnabled);
            ImGui::SliderFloat("##JumpPower", &variables::Local::jumpPower, 50.0f, 200.0f, "%.0f");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Desync", &variables::Local::desyncEnabled);
        }

        ImGui::EndChild();

        ImGui::End();
    }
    void render(ImDrawList* drawList)
    {
        static auto lastTime = std::chrono::high_resolution_clock::now();
        static int frameCount = 0;
        static int fps = 0;

        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

        if (elapsed >= 1000) {
            fps = frameCount;
            frameCount = 0;
            lastTime = currentTime;
        }

        std::string watermark = "Made by Metix | Roblox External | FPS: " + std::to_string(fps);
        ImVec2 textSize = ImGui::CalcTextSize(watermark.c_str());
        float screenW = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
        ImVec2 watermarkPos = ImVec2(screenW - textSize.x - 10, 10);

        drawList->AddText(ImVec2(watermarkPos.x - 1, watermarkPos.y), IM_COL32(0, 0, 0, 255), watermark.c_str());
        drawList->AddText(ImVec2(watermarkPos.x + 1, watermarkPos.y), IM_COL32(0, 0, 0, 255), watermark.c_str());
        drawList->AddText(ImVec2(watermarkPos.x, watermarkPos.y - 1), IM_COL32(0, 0, 0, 255), watermark.c_str());
        drawList->AddText(ImVec2(watermarkPos.x, watermarkPos.y + 1), IM_COL32(0, 0, 0, 255), watermark.c_str());
        drawList->AddText(watermarkPos, IM_COL32(255, 255, 255, 255), watermark.c_str());

        if (variables::Aimbot::enabled && variables::Aimbot::showFOV) {
            POINT p;
            GetCursorPos(&p);
            ImVec2 center = ImVec2(static_cast<float>(p.x), static_cast<float>(p.y));
            drawList->AddCircle(center, variables::Aimbot::fovRadius, IM_COL32(0, 0, 0, 255), 64, 2.0f);
            drawList->AddCircle(center, variables::Aimbot::fovRadius, IM_COL32(255, 255, 255, 255), 64, 1.0f);
        }

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
