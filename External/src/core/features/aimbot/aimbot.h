#pragma once
#include "../../../sdk/w2s.h"
#include "../../../Core/Cache/Cache.h"
#include "../../../Core/variables/variables.h"
#include "../../../sdk/sdk.h"
#include <windows.h>
#include <cmath>

namespace Aimbot {

    inline uintptr_t lockedPlayerAddr = 0;

    inline void MoveMouse(float x, float y)
    {
        // full dtc by hyperion v67 ig but trust ud
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.dx = static_cast<LONG>(x);
        input.mi.dy = static_cast<LONG>(y);
        SendInput(1, &input, sizeof(INPUT));
    }



    inline float GetDistance2D(RBX::Vec2 a, RBX::Vec2 b) {
        float dx = a.X - b.X;
        float dy = a.Y - b.Y;
        return sqrtf(dx * dx + dy * dy);
    }


    inline void RunAimbot(const RBX::Mat4& viewMatrix) {
        if (!variables::Aimbot::enabled) return;

        bool keyPressed = false;
        if (variables::Aimbot::aimbotKey == 1) keyPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
        else if (variables::Aimbot::aimbotKey == 2) keyPressed = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);
        else if (variables::Aimbot::aimbotKey == 4) keyPressed = (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
        else if (variables::Aimbot::aimbotKey == 5) keyPressed = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000);
        else if (variables::Aimbot::aimbotKey == 6) keyPressed = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000);
        else if (variables::Aimbot::aimbotKey == 16) keyPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        else if (variables::Aimbot::aimbotKey == 17) keyPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        else if (variables::Aimbot::aimbotKey == 18) keyPressed = (GetAsyncKeyState(VK_MENU) & 0x8000);
        else if (variables::Aimbot::aimbotKey > 0) keyPressed = (GetAsyncKeyState(variables::Aimbot::aimbotKey) & 0x8000);

        if (!keyPressed)
        {
            lockedPlayerAddr = 0;
            return;
        }

        float screenW = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
        float screenH = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
        //std::cout << "Screen: " << screenW << "x" << screenH << std::endl;
        // do you have an 60hz screen ig?

        POINT mousePos;
        GetCursorPos(&mousePos);
        RBX::Vec2 aimCenter = { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };

        if (lockedPlayerAddr == 0) {
            float closestDist = 999999.0f;
            RBX::Vec2 closestTarget = { 0,0 };
            uintptr_t closestPlayerAddr = 0;

            for (auto& plr : PlayerCache::players) {
                if (!plr.isValid) continue;

                auto character = RBX::RbxInstance(plr.characterAddr);
                RBX::RbxInstance targetPart = RBX::RbxInstance(0);

                if (variables::Aimbot::aimTarget == 0) {
                    targetPart = character.FindChild("Head");
                }
                else {
                    targetPart = RBX::RbxInstance(plr.rootPartAddr);
                }

                if (targetPart.Addr == 0) continue;

                RBX::Vec3 targetPos = targetPart.GetPos();
                RBX::Vec2 screenPos = W2S::WorldToScreen(targetPos, viewMatrix);

                if (screenPos.X == 0 && screenPos.Y == 0) continue;
                if (screenPos.X < 0 || screenPos.Y < 0 || screenPos.X > screenW || screenPos.Y > screenH) continue;

                float dist = GetDistance2D(aimCenter, screenPos);

                if (dist < variables::Aimbot::fovRadius && dist < closestDist) {
                    closestDist = dist;
                    closestTarget = screenPos;
                    closestPlayerAddr = plr.playerAddr;
                }
            }

            if (closestPlayerAddr != 0) {
                lockedPlayerAddr = closestPlayerAddr;
            }
        }

        if (lockedPlayerAddr != 0) {
            bool foundLockedPlayer = false;
            RBX::Vec2 targetScreenPos = { 0, 0 };

            for (auto& plr : PlayerCache::players) {
                if (!plr.isValid) continue;
                if (plr.playerAddr != lockedPlayerAddr) continue;

                auto character = RBX::RbxInstance(plr.characterAddr);
                RBX::RbxInstance targetPart = RBX::RbxInstance(0);

                if (variables::Aimbot::aimTarget == 0) {
                    targetPart = character.FindChild("Head");
                }
                else {
                    targetPart = RBX::RbxInstance(plr.rootPartAddr);
                }

                if (targetPart.Addr == 0) break;

                RBX::Vec3 targetPos = targetPart.GetPos();
                RBX::Vec2 screenPos = W2S::WorldToScreen(targetPos, viewMatrix);

                if (screenPos.X == 0 && screenPos.Y == 0) break;
                if (screenPos.X < 0 || screenPos.Y < 0 || screenPos.X > screenW || screenPos.Y > screenH) break;

                targetScreenPos = screenPos;
                foundLockedPlayer = true;
                break;
            }

            if (!foundLockedPlayer) {
                lockedPlayerAddr = 0;
                return;
            }

            float dx = targetScreenPos.X - mousePos.x;
            float dy = targetScreenPos.Y - mousePos.y;

            float smoothFactor = 1.0f / variables::Aimbot::smoothing;

            MoveMouse(dx * smoothFactor, dy * smoothFactor); // make the mouse more ud for hyperion v67
        }
    }
}
