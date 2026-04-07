#include <filesystem>
#include <cstdint>
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <atomic>
#include "src/memory/memory.h"
#include "src/sdk/offsets.h"
#include "src/sdk/sdk.h"
#include "src/core/cache/cache.h"
#include "src/core/globals/globals.h"
#include "src/core/features/visuals/visuals.h"
#include "src/render/render.h"

namespace
{
	constexpr const char* app = "RobloxPlayerBeta.exe";
	constexpr const wchar_t* apptitle = L"Roblox";

	std::atomic<bool> running(true);

	bool isgamerunning(const wchar_t* windowTitle)
	{
		HWND hwnd = FindWindowW(NULL, windowTitle);
		return hwnd != NULL;
	}

	void thread()
	{
		while (running) {
			auto character = Globals::localPlayer.GetModelRef();
			if (character.Addr == 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			auto humanoid = character.FindChildByClass("Humanoid");
			if (humanoid.Addr == 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			if (variables::Local::speedEnabled) {
				RBX::ModifyWalkSpeed(humanoid, variables::Local::walkSpeed);
			}

			if (variables::Local::jumpEnabled) {
				RBX::ModifyJumpPower(humanoid, variables::Local::jumpPower);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	bool init()
	{
		if (!memory->find_process_id(app)) {
			printf("unable to get pid.");
			return false;
		}

		if (!memory->attach_to_process(app)) {
			printf("unable to attach roblox.");
			return false;
		}

		if (!memory->find_module_address(app)) {
			printf("unable to find main module adress!");
			return false;
		}

		auto baseAddr = memory->get_module_address();
		if (!baseAddr) {
			printf("base address is null.");
			return false;
		}

		auto fakeDataModelAddr = baseAddr + Offsets::FakeDataModel::Pointer;
		auto fakeDataModel = memory->read<uintptr_t>(fakeDataModelAddr);
		if (!fakeDataModel) {
			printf("fake datamodel pointer is null.");
			return false;
		}

		auto dataModelAddr = fakeDataModel + Offsets::FakeDataModel::RealDataModel;
		auto dataModelPtr = memory->read<uintptr_t>(dataModelAddr);
		if (!dataModelPtr) {
			printf("datamodel pointer is null.");
			return false;
		}

		auto visualEngineAddr = baseAddr + Offsets::VisualEngine::Pointer;
		auto visualEngine = memory->read<uintptr_t>(visualEngineAddr);
		if (!visualEngine) {
			printf("visualengine pointer is null.");
			return false;
		}

		Globals::dataModel = RBX::RbxInstance(dataModelPtr);
		Globals::renderEngine = RBX::RenderEngine(visualEngine);
		Globals::workspace = Globals::dataModel.FindChildByClass("Workspace");
		Globals::players = Globals::dataModel.FindChildByClass("Players");
		Globals::camera = Globals::workspace.FindChildByClass("Camera");

		auto localPlayerAddr = memory->read<uintptr_t>(Globals::players.Addr + Offsets::Player::LocalPlayer);
		Globals::localPlayer = RBX::RbxInstance(localPlayerAddr);

		system("cls");
		// for debugging.
		//std::cout << "[+] DataModel: 0x" << std::hex << dataModelPtr << std::dec << "\n";
		//std::cout << "[+] VisualEngine: 0x" << std::hex << visualEngine << std::dec << "\n";
		//std::cout << "[+] Workspace: 0x" << std::hex << Globals::workspace.Addr << std::dec << "\n";
		//std::cout << "[+] Players: 0x" << std::hex << Globals::players.Addr << std::dec << "\n";
		//std::cout << "[+] Camera: 0x" << std::hex << Globals::camera.Addr << std::dec << "\n";
		//std::cout << "[+] LocalPlayer: 0x" << std::hex << localPlayerAddr << std::dec << "\n\n";

		return true;
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
	}
}


std::int32_t main() 
{
	SetConsoleTitleA("sexy");
	if (!init()) {
		return 1;
	}


	OverlayWindow overlay;
	if (!overlay.Initialize()) {
		std::cout << "[!] failed to initialize overlay\n";
		return -1;
	}

	std::cout << "[+] overlay initialized\n";
	std::cout << "[*] press insert to toggle menu\n\n";

    std::thread localThread(thread);
    int frameCounter = 0;

    while (memory->IsConnected()) {
        if (!isgamerunning(apptitle)) {
            break;
        }

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            variables::menuOpen = !variables::menuOpen;
        }

        if (frameCounter % 3 == 0) {
            PlayerCache::updateplayers();
        }
        frameCounter++;

        overlay.BeginFrame();
        overlay.RenderMenu();

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        render(drawList);

        auto viewMatrix = Globals::renderEngine.GetViewMat();
        Visuals::RenderESP(drawList, viewMatrix);

        overlay.EndFrame();
    }

    running = false;
    if (localThread.joinable()) {
        localThread.join();
    }

    overlay.Cleanup();
    return 0;
}