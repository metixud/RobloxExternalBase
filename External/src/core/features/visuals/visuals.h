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

    inline void RenderESP(ImDrawList* drawList, const RBX::Mat4& viewMatrix)
    {
        if (!variables::ESP::enabled) return;

        for (auto& plr : PlayerCache::players) {
            if (!plr.isValid) continue;

            auto character = RBX::RbxInstance(plr.characterAddr);

            auto head = character.FindChild("Head");
            auto torso = character.FindChild("Torso");
            auto leftArm = character.FindChild("Left Arm");
            auto rightArm = character.FindChild("Right Arm");
            auto leftLeg = character.FindChild("Left Leg");
            auto rightLeg = character.FindChild("Right Leg");

            bool isR6 = (torso.Addr != 0);

            if (!isR6) {
                head = character.FindChild("Head");
                torso = character.FindChild("UpperTorso");
                leftArm = character.FindChild("LeftHand");
                rightArm = character.FindChild("RightHand");
                leftLeg = character.FindChild("LeftFoot");
                rightLeg = character.FindChild("RightFoot");
            }

            if (head.Addr == 0) continue;

            RBX::Vec3 boundPoints[200];
            int boundPointCount = 0;

            if (isR6) {
                auto headPos = head.GetPos();
                const float headSize = 1.0f;
                for (int i = 0; i < 8; i++) {
                    float angle = (i / 8.0f) * 6.28318f;
                    boundPoints[boundPointCount++] = { headPos.X + cosf(angle) * headSize, headPos.Y, headPos.Z + sinf(angle) * headSize };
                }
                boundPoints[boundPointCount++] = { headPos.X, headPos.Y + headSize, headPos.Z };
                boundPoints[boundPointCount++] = { headPos.X, headPos.Y - headSize * 0.5f, headPos.Z };

                if (torso.Addr != 0) {
                    auto torsoCF = torso.GetCFrame();
                    auto torsoPos = torsoCF.GetPosition();
                    auto torsoRight = torsoCF.GetRightVector();
                    auto torsoUp = torsoCF.GetUpVector();
                    auto torsoLook = torsoCF.GetLookVector();
                    const float torsoW = 2.0f, torsoH = 2.0f, torsoD = 1.0f;

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    torsoPos.X + torsoRight.X * (x * torsoW * 0.5f) + torsoUp.X * (y * torsoH * 0.5f) + torsoLook.X * (z * torsoD * 0.5f),
                                    torsoPos.Y + torsoRight.Y * (x * torsoW * 0.5f) + torsoUp.Y * (y * torsoH * 0.5f) + torsoLook.Y * (z * torsoD * 0.5f),
                                    torsoPos.Z + torsoRight.Z * (x * torsoW * 0.5f) + torsoUp.Z * (y * torsoH * 0.5f) + torsoLook.Z * (z * torsoD * 0.5f)
                                };
                            }
                        }
                    }
                }

                const float armW = 1.0f, armH = 2.0f, armD = 1.0f;
                if (rightArm.Addr != 0) {
                    auto armCF = rightArm.GetCFrame();
                    auto armPos = armCF.GetPosition();
                    auto armRight = armCF.GetRightVector();
                    auto armUp = armCF.GetUpVector();
                    auto armLook = armCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    armPos.X + armRight.X * (x * armW * 0.5f) + armUp.X * (y * armH * 0.5f) + armLook.X * (z * armD * 0.5f),
                                    armPos.Y + armRight.Y * (x * armW * 0.5f) + armUp.Y * (y * armH * 0.5f) + armLook.Y * (z * armD * 0.5f),
                                    armPos.Z + armRight.Z * (x * armW * 0.5f) + armUp.Z * (y * armH * 0.5f) + armLook.Z * (z * armD * 0.5f)
                                };
                            }
                        }
                    }
                }

                if (leftArm.Addr != 0) {
                    auto armCF = leftArm.GetCFrame();
                    auto armPos = armCF.GetPosition();
                    auto armRight = armCF.GetRightVector();
                    auto armUp = armCF.GetUpVector();
                    auto armLook = armCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    armPos.X + armRight.X * (x * armW * 0.5f) + armUp.X * (y * armH * 0.5f) + armLook.X * (z * armD * 0.5f),
                                    armPos.Y + armRight.Y * (x * armW * 0.5f) + armUp.Y * (y * armH * 0.5f) + armLook.Y * (z * armD * 0.5f),
                                    armPos.Z + armRight.Z * (x * armW * 0.5f) + armUp.Z * (y * armH * 0.5f) + armLook.Z * (z * armD * 0.5f)
                                };
                            }
                        }
                    }
                }

                const float legW = 1.0f, legH = 2.0f, legD = 1.0f;
                if (rightLeg.Addr != 0) {
                    auto legCF = rightLeg.GetCFrame();
                    auto legPos = legCF.GetPosition();
                    auto legRight = legCF.GetRightVector();
                    auto legUp = legCF.GetUpVector();
                    auto legLook = legCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    legPos.X + legRight.X * (x * legW * 0.5f) + legUp.X * (y * legH * 0.6f) + legLook.X * (z * legD * 0.5f),
                                    legPos.Y + legRight.Y * (x * legW * 0.5f) + legUp.Y * (y * legH * 0.6f) + legLook.Y * (z * legD * 0.5f),
                                    legPos.Z + legRight.Z * (x * legW * 0.5f) + legUp.Z * (y * legH * 0.6f) + legLook.Z * (z * legD * 0.5f)
                                };
                            }
                        }
                    }
                }

                if (leftLeg.Addr != 0) {
                    auto legCF = leftLeg.GetCFrame();
                    auto legPos = legCF.GetPosition();
                    auto legRight = legCF.GetRightVector();
                    auto legUp = legCF.GetUpVector();
                    auto legLook = legCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    legPos.X + legRight.X * (x * legW * 0.5f) + legUp.X * (y * legH * 0.6f) + legLook.X * (z * legD * 0.5f),
                                    legPos.Y + legRight.Y * (x * legW * 0.5f) + legUp.Y * (y * legH * 0.6f) + legLook.Y * (z * legD * 0.5f),
                                    legPos.Z + legRight.Z * (x * legW * 0.5f) + legUp.Z * (y * legH * 0.6f) + legLook.Z * (z * legD * 0.5f)
                                };
                            }
                        }
                    }
                }
            }
            else {
                auto headPos = head.GetPos();
                const float headSize = 0.8f;
                for (int i = 0; i < 8; i++) {
                    float angle = (i / 8.0f) * 6.28318f;
                    boundPoints[boundPointCount++] = { headPos.X + cosf(angle) * headSize, headPos.Y, headPos.Z + sinf(angle) * headSize };
                }
                boundPoints[boundPointCount++] = { headPos.X, headPos.Y + headSize, headPos.Z };
                boundPoints[boundPointCount++] = { headPos.X, headPos.Y - headSize * 0.5f, headPos.Z };

                if (torso.Addr != 0) {
                    auto torsoCF = torso.GetCFrame();
                    auto torsoPos = torsoCF.GetPosition();
                    auto torsoRight = torsoCF.GetRightVector();
                    auto torsoUp = torsoCF.GetUpVector();
                    auto torsoLook = torsoCF.GetLookVector();
                    const float utW = 1.6f, utH = 1.5f, utD = 0.8f;

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    torsoPos.X + torsoRight.X * (x * utW * 0.5f) + torsoUp.X * (y * utH * 0.5f) + torsoLook.X * (z * utD * 0.5f),
                                    torsoPos.Y + torsoRight.Y * (x * utW * 0.5f) + torsoUp.Y * (y * utH * 0.5f) + torsoLook.Y * (z * utD * 0.5f),
                                    torsoPos.Z + torsoRight.Z * (x * utW * 0.5f) + torsoUp.Z * (y * utH * 0.5f) + torsoLook.Z * (z * utD * 0.5f)
                                };
                            }
                        }
                    }
                }

                const float handW = 0.8f, handH = 0.8f, handD = 0.8f;
                if (rightArm.Addr != 0) {
                    auto handCF = rightArm.GetCFrame();
                    auto handPos = handCF.GetPosition();
                    auto handRight = handCF.GetRightVector();
                    auto handUp = handCF.GetUpVector();
                    auto handLook = handCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    handPos.X + handRight.X * (x * handW * 0.5f) + handUp.X * (y * handH * 0.5f) + handLook.X * (z * handD * 0.5f),
                                    handPos.Y + handRight.Y * (x * handW * 0.5f) + handUp.Y * (y * handH * 0.5f) + handLook.Y * (z * handD * 0.5f),
                                    handPos.Z + handRight.Z * (x * handW * 0.5f) + handUp.Z * (y * handH * 0.5f) + handLook.Z * (z * handD * 0.5f)
                                };
                            }
                        }
                    }
                }

                if (leftArm.Addr != 0) {
                    auto handCF = leftArm.GetCFrame();
                    auto handPos = handCF.GetPosition();
                    auto handRight = handCF.GetRightVector();
                    auto handUp = handCF.GetUpVector();
                    auto handLook = handCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    handPos.X + handRight.X * (x * handW * 0.5f) + handUp.X * (y * handH * 0.5f) + handLook.X * (z * handD * 0.5f),
                                    handPos.Y + handRight.Y * (x * handW * 0.5f) + handUp.Y * (y * handH * 0.5f) + handLook.Y * (z * handD * 0.5f),
                                    handPos.Z + handRight.Z * (x * handW * 0.5f) + handUp.Z * (y * handH * 0.5f) + handLook.Z * (z * handD * 0.5f)
                                };
                            }
                        }
                    }
                }

                const float footW = 0.8f, footH = 0.7f, footD = 1.2f;
                if (rightLeg.Addr != 0) {
                    auto footCF = rightLeg.GetCFrame();
                    auto footPos = footCF.GetPosition();
                    auto footRight = footCF.GetRightVector();
                    auto footUp = footCF.GetUpVector();
                    auto footLook = footCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    footPos.X + footRight.X * (x * footW * 0.5f) + footUp.X * (y * footH * 0.5f) + footLook.X * (z * footD * 0.5f),
                                    footPos.Y + footRight.Y * (x * footW * 0.5f) + footUp.Y * (y * footH * 0.5f) + footLook.Y * (z * footD * 0.5f),
                                    footPos.Z + footRight.Z * (x * footW * 0.5f) + footUp.Z * (y * footH * 0.5f) + footLook.Z * (z * footD * 0.5f)
                                };
                            }
                        }
                    }
                }

                if (leftLeg.Addr != 0) {
                    auto footCF = leftLeg.GetCFrame();
                    auto footPos = footCF.GetPosition();
                    auto footRight = footCF.GetRightVector();
                    auto footUp = footCF.GetUpVector();
                    auto footLook = footCF.GetLookVector();

                    for (int x = -1; x <= 1; x += 2) {
                        for (int y = -1; y <= 1; y += 2) {
                            for (int z = -1; z <= 1; z += 2) {
                                boundPoints[boundPointCount++] = {
                                    footPos.X + footRight.X * (x * footW * 0.5f) + footUp.X * (y * footH * 0.5f) + footLook.X * (z * footD * 0.5f),
                                    footPos.Y + footRight.Y * (x * footW * 0.5f) + footUp.Y * (y * footH * 0.5f) + footLook.Y * (z * footD * 0.5f),
                                    footPos.Z + footRight.Z * (x * footW * 0.5f) + footUp.Z * (y * footH * 0.5f) + footLook.Z * (z * footD * 0.5f)
                                };
                            }
                        }
                    }
                }
            }

            float minX = 999999.0f, minY = 999999.0f;
            float maxX = -999999.0f, maxY = -999999.0f;
            int validPoints = 0;

            for (int i = 0; i < boundPointCount; i++) {
                RBX::Vec2 screenPos = W2S::WorldToScreen(boundPoints[i], viewMatrix);
                if (screenPos.X != 0 || screenPos.Y != 0) {
                    minX = (std::min)(minX, screenPos.X);
                    minY = (std::min)(minY, screenPos.Y);
                    maxX = (std::max)(maxX, screenPos.X);
                    maxY = (std::max)(maxY, screenPos.Y);
                    validPoints++;
                }
            }

            if (validPoints < 3 || minX == 999999.0f) continue;

            float boxWidth = maxX - minX;
            float boxHeight = maxY - minY;

            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            const float MAX_BOX_WIDTH = screenSize.x * 1.5f;
            const float MAX_BOX_HEIGHT = screenSize.y * 1.5f;

            if (boxWidth > MAX_BOX_WIDTH || boxHeight > MAX_BOX_HEIGHT) continue;

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
        }
    }
}
