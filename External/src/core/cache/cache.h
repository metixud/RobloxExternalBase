#pragma once
#include "../../../src/sdk/sdk.h"
#include "../globals/Globals.h"
#include "../variables/variables.h"
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
        uintptr_t teamAddr;

        std::string name;
        RBX::Vec3 position;
        int health;
        int maxHealth;
        float distance;

        bool isValid;
    };

    inline std::vector<CachedPlayer> players;
    inline RBX::Vec3 localPlayerPos;
    inline uintptr_t localPlayerTeam = 0;

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
        localPlayerTeam = memory->read<uintptr_t>(Globals::localPlayer.Addr + Offsets::Player::Team);

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

            int health = memory->read<int>(humanoid.Addr + Offsets::Humanoid::Health);
            
            if (variables::ESP::deadCheck && health <= 0) continue;

            uintptr_t teamAddr = memory->read<uintptr_t>(plr.Addr + Offsets::Player::Team);
            
            if (variables::teamCheck && teamAddr != 0 && teamAddr == localPlayerTeam) continue;

            CachedPlayer cachedPlayer{};
            cachedPlayer.playerAddr = plr.Addr;
            cachedPlayer.characterAddr = character.Addr;
            cachedPlayer.humanoidAddr = humanoid.Addr;
            cachedPlayer.rootPartAddr = rootPart.Addr;
            cachedPlayer.teamAddr = teamAddr;
            cachedPlayer.name = plr.GetName();
            cachedPlayer.position = rootPart.GetPos();
            cachedPlayer.health = health;
            cachedPlayer.maxHealth = memory->read<int>(humanoid.Addr + Offsets::Humanoid::MaxHealth);
            cachedPlayer.distance = rootPart.CalcDistance(localPlayerPos);
            cachedPlayer.isValid = true;

            updatedPlayers.push_back(std::move(cachedPlayer));
        }

        players = std::move(updatedPlayers);

    }
}
