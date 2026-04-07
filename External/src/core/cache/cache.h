#pragma once
#include "../../../src/sdk/sdk.h"
#include "../globals/Globals.h"
#include <vector>
#include <string>
#include <algorithm>

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
        auto playerList = Globals::players.GetChildList();

        auto localChar = Globals::localPlayer.GetModelRef();
        if (localChar.Addr == 0) return;

        auto localRoot = localChar.FindChild("HumanoidRootPart");
        if (localRoot.Addr == 0) return;

        localPlayerPos = localRoot.GetPos();

        for (auto& cached : players) {
            cached.isValid = false;
        }

        for (auto& plr : playerList) {
            if (plr.Addr == Globals::localPlayer.Addr) continue;

            CachedPlayer* existingPlayer = nullptr;
            for (auto& cached : players) {
                if (cached.playerAddr == plr.Addr) {
                    existingPlayer = &cached;
                    break;
                }
            }

            if (existingPlayer == nullptr) {
                CachedPlayer newPlayer;
                newPlayer.playerAddr = plr.Addr;
                newPlayer.name = plr.GetName();
                players.push_back(newPlayer);
                existingPlayer = &players.back();
            }

            auto character = plr.GetModelRef();
            if (character.Addr == 0) continue;

            existingPlayer->characterAddr = character.Addr;

            auto humanoid = character.FindChildByClass("Humanoid");
            if (humanoid.Addr == 0) continue;

            existingPlayer->humanoidAddr = humanoid.Addr;

            auto rootPart = character.FindChild("HumanoidRootPart");
            if (rootPart.Addr == 0) continue;

            existingPlayer->rootPartAddr = rootPart.Addr;
            existingPlayer->position = rootPart.GetPos();
            existingPlayer->health = memory->read<int>(humanoid.Addr + Offsets::Humanoid::Health);
            existingPlayer->maxHealth = memory->read<int>(humanoid.Addr + Offsets::Humanoid::MaxHealth);
            existingPlayer->distance = rootPart.CalcDistance(localPlayerPos);
            existingPlayer->isValid = true;
        }

        players.erase(
            std::remove_if(players.begin(), players.end(),
                [](const CachedPlayer& p) { return !p.isValid; }),
            players.end()
        );

    }
}
