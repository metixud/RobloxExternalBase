#pragma once
#include "../../../../src/sdk/w2s.h"
#include "../../../../src/core/cache/cache.h"
#include "../../../../src/core/variables/variables.h"
#include "../../../../ext/imgui/imgui.h"
#include <string>
#include <algorithm>
#include <cmath>

namespace Visuals {

    inline void DrawOutlinedText(ImDrawList* drawList, const ImVec2& pos, const std::string& text, ImU32 textColor) {
        drawList->AddText(ImVec2(pos.x - 1, pos.y), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(ImVec2(pos.x + 1, pos.y), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(ImVec2(pos.x, pos.y - 1), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(ImVec2(pos.x, pos.y + 1), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(pos, textColor, text.c_str());
    }

    inline void DrawLine(ImDrawList* drawList, const ImVec2& start, const ImVec2& end, ImU32 color, float thickness, bool outline) {
        if (outline) {
            drawList->AddLine(start, end, IM_COL32(0, 0, 0, 255), thickness + 2.0f);
        }
        drawList->AddLine(start, end, color, thickness);
    }

    inline void DrawSkeletonBone(ImDrawList* drawList, const RBX::Vec3& pos1, const RBX::Vec3& pos2, 
                                  const RBX::Mat4& viewMatrix, ImU32 color, float thickness, bool outline) {
        RBX::Vec2 screenPos1 = W2S::WorldToScreen(pos1, viewMatrix);
        RBX::Vec2 screenPos2 = W2S::WorldToScreen(pos2, viewMatrix);

        if ((screenPos1.X != 0 || screenPos1.Y != 0) && (screenPos2.X != 0 || screenPos2.Y != 0)) {
            ImVec2 start(screenPos1.X, screenPos1.Y);
            ImVec2 end(screenPos2.X, screenPos2.Y);
            DrawLine(drawList, start, end, color, thickness, outline);
        }
    }

    inline void RenderSkeleton(ImDrawList* drawList, RBX::RbxInstance character, 
                                const RBX::Mat4& viewMatrix, bool isR6) {
        ImU32 boneColor = IM_COL32(255, 255, 255, 255);
        float thickness = variables::ESP::skeletonThickness;
        bool outline = variables::ESP::skeletonOutline;

        if (isR6) {
            auto head = character.FindChild("Head");
            auto torso = character.FindChild("Torso");
            auto leftArm = character.FindChild("Left Arm");
            auto rightArm = character.FindChild("Right Arm");
            auto leftLeg = character.FindChild("Left Leg");
            auto rightLeg = character.FindChild("Right Leg");

            if (head.Addr == 0 || torso.Addr == 0) return;

            RBX::Vec3 headPos = head.GetPos();
            RBX::Vec3 torsoPos = torso.GetPos();

            DrawSkeletonBone(drawList, headPos, torsoPos, viewMatrix, boneColor, thickness, outline);

            if (leftArm.Addr != 0) {
                RBX::Vec3 leftArmPos = leftArm.GetPos();
                DrawSkeletonBone(drawList, torsoPos, leftArmPos, viewMatrix, boneColor, thickness, outline);
            }

            if (rightArm.Addr != 0) {
                RBX::Vec3 rightArmPos = rightArm.GetPos();
                DrawSkeletonBone(drawList, torsoPos, rightArmPos, viewMatrix, boneColor, thickness, outline);
            }

            if (leftLeg.Addr != 0) {
                RBX::Vec3 leftLegPos = leftLeg.GetPos();
                DrawSkeletonBone(drawList, torsoPos, leftLegPos, viewMatrix, boneColor, thickness, outline);
            }

            if (rightLeg.Addr != 0) {
                RBX::Vec3 rightLegPos = rightLeg.GetPos();
                DrawSkeletonBone(drawList, torsoPos, rightLegPos, viewMatrix, boneColor, thickness, outline);
            }
        }
        else {
            auto head = character.FindChild("Head");
            auto upperTorso = character.FindChild("UpperTorso");
            auto lowerTorso = character.FindChild("LowerTorso");

            auto leftUpperArm = character.FindChild("LeftUpperArm");
            auto leftLowerArm = character.FindChild("LeftLowerArm");
            auto leftHand = character.FindChild("LeftHand");

            auto rightUpperArm = character.FindChild("RightUpperArm");
            auto rightLowerArm = character.FindChild("RightLowerArm");
            auto rightHand = character.FindChild("RightHand");

            auto leftUpperLeg = character.FindChild("LeftUpperLeg");
            auto leftLowerLeg = character.FindChild("LeftLowerLeg");
            auto leftFoot = character.FindChild("LeftFoot");

            auto rightUpperLeg = character.FindChild("RightUpperLeg");
            auto rightLowerLeg = character.FindChild("RightLowerLeg");
            auto rightFoot = character.FindChild("RightFoot");

            if (head.Addr == 0 || upperTorso.Addr == 0) return;

            RBX::Vec3 headPos = head.GetPos();
            RBX::Vec3 upperTorsoPos = upperTorso.GetPos();

            DrawSkeletonBone(drawList, headPos, upperTorsoPos, viewMatrix, boneColor, thickness, outline);

            if (lowerTorso.Addr != 0) {
                RBX::Vec3 lowerTorsoPos = lowerTorso.GetPos();
                DrawSkeletonBone(drawList, upperTorsoPos, lowerTorsoPos, viewMatrix, boneColor, thickness, outline);

                if (leftUpperArm.Addr != 0) {
                    RBX::Vec3 leftUpperArmPos = leftUpperArm.GetPos();
                    DrawSkeletonBone(drawList, upperTorsoPos, leftUpperArmPos, viewMatrix, boneColor, thickness, outline);

                    if (leftLowerArm.Addr != 0) {
                        RBX::Vec3 leftLowerArmPos = leftLowerArm.GetPos();
                        DrawSkeletonBone(drawList, leftUpperArmPos, leftLowerArmPos, viewMatrix, boneColor, thickness, outline);

                        if (leftHand.Addr != 0) {
                            RBX::Vec3 leftHandPos = leftHand.GetPos();
                            DrawSkeletonBone(drawList, leftLowerArmPos, leftHandPos, viewMatrix, boneColor, thickness, outline);
                        }
                    }
                }

                if (rightUpperArm.Addr != 0) {
                    RBX::Vec3 rightUpperArmPos = rightUpperArm.GetPos();
                    DrawSkeletonBone(drawList, upperTorsoPos, rightUpperArmPos, viewMatrix, boneColor, thickness, outline);

                    if (rightLowerArm.Addr != 0) {
                        RBX::Vec3 rightLowerArmPos = rightLowerArm.GetPos();
                        DrawSkeletonBone(drawList, rightUpperArmPos, rightLowerArmPos, viewMatrix, boneColor, thickness, outline);

                        if (rightHand.Addr != 0) {
                            RBX::Vec3 rightHandPos = rightHand.GetPos();
                            DrawSkeletonBone(drawList, rightLowerArmPos, rightHandPos, viewMatrix, boneColor, thickness, outline);
                        }
                    }
                }

                if (leftUpperLeg.Addr != 0) {
                    RBX::Vec3 leftUpperLegPos = leftUpperLeg.GetPos();
                    DrawSkeletonBone(drawList, lowerTorsoPos, leftUpperLegPos, viewMatrix, boneColor, thickness, outline);

                    if (leftLowerLeg.Addr != 0) {
                        RBX::Vec3 leftLowerLegPos = leftLowerLeg.GetPos();
                        DrawSkeletonBone(drawList, leftUpperLegPos, leftLowerLegPos, viewMatrix, boneColor, thickness, outline);

                        if (leftFoot.Addr != 0) {
                            RBX::Vec3 leftFootPos = leftFoot.GetPos();
                            DrawSkeletonBone(drawList, leftLowerLegPos, leftFootPos, viewMatrix, boneColor, thickness, outline);
                        }
                    }
                }

                if (rightUpperLeg.Addr != 0) {
                    RBX::Vec3 rightUpperLegPos = rightUpperLeg.GetPos();
                    DrawSkeletonBone(drawList, lowerTorsoPos, rightUpperLegPos, viewMatrix, boneColor, thickness, outline);

                    if (rightLowerLeg.Addr != 0) {
                        RBX::Vec3 rightLowerLegPos = rightLowerLeg.GetPos();
                        DrawSkeletonBone(drawList, rightUpperLegPos, rightLowerLegPos, viewMatrix, boneColor, thickness, outline);

                        if (rightFoot.Addr != 0) {
                            RBX::Vec3 rightFootPos = rightFoot.GetPos();
                            DrawSkeletonBone(drawList, rightLowerLegPos, rightFootPos, viewMatrix, boneColor, thickness, outline);
                        }
                    }
                }
            }
        }
    }

    inline void RenderESP(ImDrawList* drawList, const RBX::Mat4& viewMatrix)
    {
        if (!variables::ESP::enabled) return;

        for (auto& plr : PlayerCache::players) {
            if (!plr.isValid) continue;

            auto character = RBX::RbxInstance(plr.characterAddr);
            auto head = character.FindChild("Head");
            
            if (head.Addr == 0) continue;

            auto torso = character.FindChild("Torso");
            bool isR6 = (torso.Addr != 0);

            RBX::RbxInstance hrp = character.FindChild("HumanoidRootPart");
            if (hrp.Addr == 0) continue;

            if (hrp.Addr == 0) continue;

            RBX::Vec3 headPos = head.GetPos();
            RBX::Vec3 hrpPos = hrp.GetPos();

            RBX::Vec2 headScreen = W2S::WorldToScreen(headPos, viewMatrix);
            RBX::Vec2 hrpScreen = W2S::WorldToScreen(hrpPos, viewMatrix);

            if ((headScreen.X == 0 && headScreen.Y == 0) || (hrpScreen.X == 0 && hrpScreen.Y == 0)) continue;

            float headYOffset = isR6 ? 0.5f : 0.5f;
            float feetYOffset = isR6 ? 3.0f : 2.5f;
            
            RBX::Vec3 topPos = { headPos.X, headPos.Y + headYOffset, headPos.Z };
            RBX::Vec3 bottomPos = { hrpPos.X, hrpPos.Y - feetYOffset, hrpPos.Z };
            
            RBX::Vec2 topScreen = W2S::WorldToScreen(topPos, viewMatrix);
            RBX::Vec2 bottomScreen = W2S::WorldToScreen(bottomPos, viewMatrix);

            if ((topScreen.X == 0 && topScreen.Y == 0) || (bottomScreen.X == 0 && bottomScreen.Y == 0)) continue;

            float height = bottomScreen.Y - topScreen.Y;
            float width = height * 0.4f;

            float minX = topScreen.X - width / 2.0f;
            float minY = topScreen.Y;
            float maxX = topScreen.X + width / 2.0f;
            float maxY = bottomScreen.Y;

            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            if (minX < -500 || minY < -500 || maxX > screenSize.x + 500 || maxY > screenSize.y + 500) continue;

            if (variables::ESP::skeleton) {
                RenderSkeleton(drawList, character, viewMatrix, isR6);
            }

            if (variables::ESP::boxes) {
                drawList->AddRect(
                    ImVec2(minX, minY),
                    ImVec2(maxX, maxY),
                    IM_COL32(0, 0, 0, 255),
                    0.0f, 0, 3.0f
                );

                drawList->AddRect(
                    ImVec2(minX, minY),
                    ImVec2(maxX, maxY),
                    IM_COL32(255, 255, 255, 255),
                    0.0f, 0, 1.0f
                );

                drawList->AddRect(
                    ImVec2(minX + 1, minY + 1),
                    ImVec2(maxX - 1, maxY - 1),
                    IM_COL32(0, 0, 0, 255),
                    0.0f, 0, 1.0f
                );
            }

            if (variables::ESP::healthBar && plr.maxHealth > 0) {
                float healthPercent = static_cast<float>(plr.health) / static_cast<float>(plr.maxHealth);
                ImU32 healthColor = IM_COL32(255 * (1 - healthPercent), 255 * healthPercent, 0, 255);

                float barHeight = (maxY - minY) * healthPercent;

                drawList->AddRectFilled(
                    ImVec2(minX - 6, minY),
                    ImVec2(minX - 2, maxY),
                    IM_COL32(0, 0, 0, 200)
                );

                drawList->AddRectFilled(
                    ImVec2(minX - 5, maxY - barHeight),
                    ImVec2(minX - 3, maxY),
                    healthColor
                );
            }

            if (variables::ESP::names) {
                ImVec2 textSize = ImGui::CalcTextSize(plr.name.c_str());
                float textX = (minX + maxX) / 2.0f - textSize.x / 2.0f;
                float textY = minY - textSize.y - 2;
                DrawOutlinedText(drawList, ImVec2(textX, textY), plr.name, IM_COL32(255, 255, 255, 255));
            }

            if (variables::ESP::distance) {
                std::string distText = std::to_string(static_cast<int>(plr.distance)) + "m";
                ImVec2 textSize = ImGui::CalcTextSize(distText.c_str());
                float textX = (minX + maxX) / 2.0f - textSize.x / 2.0f;
                float textY = maxY + 2;
                DrawOutlinedText(drawList, ImVec2(textX, textY), distText, IM_COL32(255, 255, 255, 255));
            }

            if (variables::ESP::snaplines) {
                ImU32 color = IM_COL32(255, 255, 255, 255);
                ImVec2 screenSize = ImGui::GetIO().DisplaySize;
                ImVec2 origin_pos, destination_pos;

                switch (variables::ESP::snaplinesOrigin) {
                case 0: {
                    POINT cursorPos;
                    if (GetCursorPos(&cursorPos)) {
                        HWND hwnd = GetForegroundWindow();
                        if (ScreenToClient(hwnd, &cursorPos)) {
                            origin_pos = ImVec2((float)cursorPos.x, (float)cursorPos.y);
                        } else {
                            origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                        }
                    } else {
                        origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                    }
                    break;
                }
                case 1: origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f); break;
                case 2: origin_pos = ImVec2(screenSize.x * 0.5f, 0.f); break;
                case 3: origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y); break;
                case 4: {
                    auto localChar = RBX::RbxInstance(Globals::localPlayer.GetModelRef());
                    auto localHead = localChar.FindChild("Head");
                    if (localHead.Addr != 0) {
                        auto headPos = W2S::WorldToScreen(localHead.GetPos(), viewMatrix);
                        origin_pos = ImVec2(headPos.X, headPos.Y);
                    } else {
                        origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                    }
                    break;
                }
                case 5: {
                    auto localChar = RBX::RbxInstance(Globals::localPlayer.GetModelRef());
                    auto localHRP = localChar.FindChild("HumanoidRootPart");
                    if (localHRP.Addr != 0) {
                        auto hrpPos = W2S::WorldToScreen(localHRP.GetPos(), viewMatrix);
                        origin_pos = ImVec2(hrpPos.X, hrpPos.Y);
                    } else {
                        origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                    }
                    break;
                }
                default: origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y); break;
                }

                switch (variables::ESP::snaplinesDestination) {
                case 0: {
                    RBX::Vec2 headScreenPos = W2S::WorldToScreen(headPos, viewMatrix);
                    destination_pos = ImVec2(headScreenPos.X, headScreenPos.Y);
                    break;
                }
                case 1: {
                    RBX::Vec2 hrpScreenPos = W2S::WorldToScreen(hrpPos, viewMatrix);
                    destination_pos = ImVec2(hrpScreenPos.X, hrpScreenPos.Y);
                    break;
                }
                case 2: {
                    ImVec2 best = ImVec2((minX + maxX) * 0.5f, (minY + maxY) * 0.5f);
                    float closest = FLT_MAX;
                    
                    RBX::Vec2 headScreenPos = W2S::WorldToScreen(headPos, viewMatrix);
                    ImVec2 headScreen = ImVec2(headScreenPos.X, headScreenPos.Y);
                    auto delta = ImVec2(origin_pos.x - headScreen.x, origin_pos.y - headScreen.y);
                    float distance = delta.x * delta.x + delta.y * delta.y;
                    if (distance < closest) {
                        closest = distance;
                        best = headScreen;
                    }

                    RBX::Vec2 hrpScreenPos2 = W2S::WorldToScreen(hrpPos, viewMatrix);
                    ImVec2 hrpScreen = ImVec2(hrpScreenPos2.X, hrpScreenPos2.Y);
                    delta = ImVec2(origin_pos.x - hrpScreen.x, origin_pos.y - hrpScreen.y);
                    distance = delta.x * delta.x + delta.y * delta.y;
                    if (distance < closest) {
                        closest = distance;
                        best = hrpScreen;
                    }

                    destination_pos = best;
                    break;
                }
                default: {
                    RBX::Vec2 headScreenPos = W2S::WorldToScreen(headPos, viewMatrix);
                    destination_pos = ImVec2(headScreenPos.X, headScreenPos.Y);
                    break;
                }
                }

                switch (variables::ESP::snaplinesStyle) {
                case 0: {
                    DrawLine(drawList, origin_pos, destination_pos, color, variables::ESP::snaplinesThickness, variables::ESP::snaplinesOutline);
                    break;
                }
                case 1: {
                    auto drop = 180.f;
                    auto segments = 24;
                    ImVec2 prev = origin_pos;
                    auto control = ImVec2(
                        (origin_pos.x + destination_pos.x) * 0.5f,
                        (origin_pos.y + destination_pos.y) * 0.5f + drop
                    );
                    for (auto i = 1; i <= segments; ++i) {
                        auto t = (float)(i) / segments;
                        auto it = 1.0f - t;
                        ImVec2 point;
                        point.x = it * it * origin_pos.x + 2 * it * t * control.x + t * t * destination_pos.x;
                        point.y = it * it * origin_pos.y + 2 * it * t * control.y + t * t * destination_pos.y;
                        DrawLine(drawList, prev, point, color, variables::ESP::snaplinesThickness, variables::ESP::snaplinesOutline);
                        prev = point;
                    }
                    break;
                }
                case 2: {
                    auto delta = ImVec2(destination_pos.x - origin_pos.x, destination_pos.y - origin_pos.y);
                    auto length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                    auto dir = ImVec2(delta.x / length, delta.y / length);
                    auto dash_len = length / (10 * 2.f);
                    for (auto i = 0; i < 10; ++i) {
                        auto start = i * 2.f * dash_len;
                        auto end = start + dash_len;
                        auto s = ImVec2(origin_pos.x + dir.x * start, origin_pos.y + dir.y * start);
                        auto e = ImVec2(origin_pos.x + dir.x * end, origin_pos.y + dir.y * end);
                        DrawLine(drawList, s, e, color, variables::ESP::snaplinesThickness, variables::ESP::snaplinesOutline);
                    }
                    break;
                }
                }
            }
        }
    }
}
