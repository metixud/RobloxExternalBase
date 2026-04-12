#include "tp_handler.h"

#include <chrono>
#include <thread>
#include <windows.h>

#include "../globals/globals.h"
#include "../../memory/memory.h"
#include "../../sdk/offsets.h"
#include "../../sdk/sdk.h"

namespace {
	void reset_globals()
	{
		Globals::dataModel = RBX::RbxInstance(0);
		Globals::renderEngine = RBX::RenderEngine(0);
		Globals::workspace = RBX::RbxInstance(0);
		Globals::players = RBX::RbxInstance(0);
		Globals::camera = RBX::RbxInstance(0);
		Globals::localPlayer = RBX::RbxInstance(0);
	}
}

void Core::tp_handler::thread()
{
	while (Globals::running) {
		if (!memory->IsConnected()) {
			Globals::running = false;
			break;
		}

		if (!FindWindowW(NULL, L"Roblox")) {
			Globals::running = false;
			break;
		}

		auto baseAddr = memory->get_module_address();
		if (!baseAddr) {
			reset_globals();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		auto fakeDataModelAddr = baseAddr + Offsets::FakeDataModel::Pointer;
		auto fakeDataModel = memory->read<uintptr_t>(fakeDataModelAddr);
		if (!fakeDataModel) {
			reset_globals();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		auto dataModelAddr = fakeDataModel + Offsets::FakeDataModel::RealDataModel;
		auto dataModelPtr = memory->read<uintptr_t>(dataModelAddr);
		auto visualEngineAddr = baseAddr + Offsets::VisualEngine::Pointer;
		auto visualEngine = memory->read<uintptr_t>(visualEngineAddr);
		if (!dataModelPtr || !visualEngine) {
			reset_globals();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		Globals::dataModel = RBX::RbxInstance(dataModelPtr);
		Globals::renderEngine = RBX::RenderEngine(visualEngine);
		Globals::workspace = Globals::dataModel.FindChildByClass("Workspace");
		Globals::players = Globals::dataModel.FindChildByClass("Players");
		Globals::camera = Globals::workspace.FindChildByClass("Camera");

		if (Globals::workspace.Addr == 0 || Globals::players.Addr == 0 || Globals::camera.Addr == 0) {
			Globals::localPlayer = RBX::RbxInstance(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		if (Globals::players.Addr != 0) {
			auto localPlayerAddr = memory->read<uintptr_t>(Globals::players.Addr + Offsets::Player::LocalPlayer);
			Globals::localPlayer = RBX::RbxInstance(localPlayerAddr);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
