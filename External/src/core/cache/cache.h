#pragma once
#include "../../../src/sdk/sdk.h"
#include "../globals/Globals.h"
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <cmath>

namespace PlayerCache {

    struct CachedPlayer {
        uintptr_t playerAddr;
        uintptr_t characterAddr;
        uintptr_t humanoidAddr;
        uintptr_t rootPartAddr;

        std::string name;
        RBX::Vec3 position;
        int health;
        int maxHealth;
        float distance;

        bool isValid;
    };

    inline std::vector<CachedPlayer> players;
    inline RBX::Vec3 localPlayerPos;

    inline void updateplayers() {
        if (Globals::players.Addr == 0 || Globals::localPlayer.Addr == 0) {
            players.clear();
            return;
        }

        auto playerList = Globals::players.GetChildList();

        auto localChar = Globals::localPlayer.GetModelRef();
        if (localChar.Addr == 0) {
            players.clear();
            return;
        }

        auto localRoot = localChar.FindChild("HumanoidRootPart");
        if (localRoot.Addr == 0) {
            players.clear();
            return;
        }

        localPlayerPos = localRoot.GetPos();

        std::vector<CachedPlayer> updatedPlayers;
        updatedPlayers.reserve(playerList.size());

        for (auto& plr : playerList) {
            if (plr.Addr == Globals::localPlayer.Addr) continue;

            auto character = plr.GetModelRef();
            if (character.Addr == 0) continue;

            auto humanoid = character.FindChildByClass("Humanoid");
            if (humanoid.Addr == 0) continue;

            auto rootPart = character.FindChild("HumanoidRootPart");
            if (rootPart.Addr == 0) continue;

            CachedPlayer cachedPlayer{};
            cachedPlayer.playerAddr = plr.Addr;
            cachedPlayer.characterAddr = character.Addr;
            cachedPlayer.humanoidAddr = humanoid.Addr;
            cachedPlayer.rootPartAddr = rootPart.Addr;
            cachedPlayer.name = plr.GetName();
            cachedPlayer.position = rootPart.GetPos();
            cachedPlayer.health = memory->read<int>(humanoid.Addr + Offsets::Humanoid::Health);
            cachedPlayer.maxHealth = memory->read<int>(humanoid.Addr + Offsets::Humanoid::MaxHealth);
            cachedPlayer.distance = rootPart.CalcDistance(localPlayerPos);
            cachedPlayer.isValid = true;

            updatedPlayers.push_back(std::move(cachedPlayer));
        }

        players = std::move(updatedPlayers);

    }
}
