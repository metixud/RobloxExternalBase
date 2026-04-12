#pragma once
#include "../../src/sdk/offsets.h"
#include "../../src/memory/memory.h"
#include <string>
#include <vector>
#include <cmath>

namespace RBX {

    struct Vec2 {
        float X{ 0 };
        float Y{ 0 };
    };

    struct Vec3 {
        float X{ 0 };
        float Y{ 0 };

        float Z{ 0 };
    };

    struct Vec4 {
        float X{ 0 };
        float Y{ 0 };
        float Z{ 0 };
        float W{ 0 };
    };


    struct Mat4 {
        float data[16];
    };

    struct CFrame {
        float data[12];

        Vec3 GetRightVector() const {
            return { data[0], data[3], data[6] };
        }

        Vec3 GetUpVector() const {
            return { data[1], data[4], data[7] };
        }

        Vec3 GetLookVector() const {
            return { data[2], data[5], data[8] };
        }

        Vec3 GetPosition() const {
            return { data[9], data[10], data[11] };
        }
    };

    class RbxInstance {
    public:
        uintptr_t Addr;

        RbxInstance(uintptr_t addr) : Addr(addr) {}

        std::string GetName() {
            uintptr_t namePtr = memory->read<uintptr_t>(Addr + Offsets::Instance::Name);
            if (namePtr == 0) return "";

            return memory->read_string(namePtr);
        }

        std::string GetClass() {
            uintptr_t classDesc = memory->read<uintptr_t>(Addr + Offsets::Instance::ClassDescriptor);
            if (classDesc == 0) return "";


            uintptr_t namePtr = memory->read<uintptr_t>(classDesc + Offsets::Instance::ClassName);
            if (namePtr == 0) return "";

            return memory->read_string(namePtr);
        }

        RbxInstance GetParent() {
            return RbxInstance(memory->read<uintptr_t>(Addr + Offsets::Instance::Parent));
        }


        std::vector<RbxInstance> GetChildList() {
            std::vector<RbxInstance> childList;

            if (Addr == 0) return childList;

            uintptr_t childStart = memory->read<uintptr_t>(Addr + Offsets::Instance::ChildrenStart);
            if (childStart == 0) return childList;

            uintptr_t childEnd = memory->read<uintptr_t>(childStart + Offsets::Instance::ChildrenEnd);
            uintptr_t current = memory->read<uintptr_t>(childStart);

            if (childEnd == 0 || current == 0 || childEnd < current) return childList;

            constexpr size_t maxChildren = 4096;
            size_t childCount = 0;

            for (uintptr_t ptr = current; ptr < childEnd && childCount < maxChildren; ptr += 0x10, ++childCount) {
                uintptr_t childAddr = memory->read<uintptr_t>(ptr);
                if (childAddr == 0) continue;
                childList.emplace_back(childAddr);
            }

            return childList;
        }

        RbxInstance FindChild(const std::string& targetName) {
            auto children = GetChildList();


            for (auto& child : children) {
                if (child.GetName() == targetName) {
                    return child;


                }
            }

            return RbxInstance(0);

        }

        RbxInstance FindChildByClass(const std::string& targetClass) {
            auto children = GetChildList();

            for (auto& child : children) {

                if (child.GetClass() == targetClass) {
                    return child;
                }
            }

            return RbxInstance(0);
        }



        RbxInstance WaitChild(const std::string& targetName) {
            while (true) {
                auto children = GetChildList();


                for (auto& child : children) {
                    if (child.GetName() == targetName) {
                        return child;
                    }
                }
            }
        }

        uintptr_t GetPrimitivePtr() {
            return memory->read<uintptr_t>(Addr + Offsets::BasePart::Primitive);
        }


        Vec3 GetPos() {
            uintptr_t prim = GetPrimitivePtr();
            if (prim == 0) return {};
            return memory->read<Vec3>(prim + Offsets::Primitive::Position);

        }

        CFrame GetCFrame() {
            uintptr_t prim = GetPrimitivePtr();
            if (prim == 0) return {};
            return memory->read<CFrame>(prim + Offsets::Primitive::Rotation);
        }

        RbxInstance GetModelRef() {
            return RbxInstance(memory->read<uintptr_t>(Addr + Offsets::Player::ModelInstance));
        }


        float CalcDistance(const Vec3& targetPos) {
            Vec3 currentPos = GetPos();


            float dx = currentPos.X - targetPos.X;
            float dy = currentPos.Y - targetPos.Y;
            float dz = currentPos.Z - targetPos.Z;

            return sqrtf(dx * dx + dy * dy + dz * dz);
        }
    };


    class RenderEngine : public RbxInstance {
    public:
        RenderEngine(uintptr_t addr) : RbxInstance(addr) {}

        Mat4 GetViewMat() {
            return memory->read<Mat4>(Addr + Offsets::VisualEngine::ViewMatrix);
        }


        Vec2 WorldToViewport(const Vec3& worldPos) {
            Vec4 quat;

            Vec2 screenDims{ static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
                            static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) };

            Mat4 viewMat = GetViewMat();

            quat.X = (worldPos.X * viewMat.data[0]) + (worldPos.Y * viewMat.data[1]) + (worldPos.Z * viewMat.data[2]) + viewMat.data[3];
            quat.Y = (worldPos.X * viewMat.data[4]) + (worldPos.Y * viewMat.data[5]) + (worldPos.Z * viewMat.data[6]) + viewMat.data[7];

            quat.Z = (worldPos.X * viewMat.data[8]) + (worldPos.Y * viewMat.data[9]) + (worldPos.Z * viewMat.data[10]) + viewMat.data[11];
            quat.W = (worldPos.X * viewMat.data[12]) + (worldPos.Y * viewMat.data[13]) + (worldPos.Z * viewMat.data[14]) + viewMat.data[15];

            Vec2 screenPos;

            if (quat.W < 0.1f) {

                return screenPos;
            }


            Vec3 ndc;
            ndc.X = quat.X / quat.W;
            ndc.Y = quat.Y / quat.W;
            ndc.Z = quat.Z / quat.W;

            screenPos.X = (screenDims.X / 2.0f * ndc.X) + (screenDims.X / 2.0f);
            screenPos.Y = -(screenDims.Y / 2.0f * ndc.Y) + (screenDims.Y / 2.0f);


            return screenPos;
        }
    };

    inline void ModifyWalkSpeed(RbxInstance humanoid, float newSpeed) {
        memory->write(humanoid.Addr + Offsets::Humanoid::Walkspeed, newSpeed);
        memory->write(humanoid.Addr + Offsets::Humanoid::WalkspeedCheck, newSpeed);

    }

    inline void ModifyJumpPower(RbxInstance humanoid, float newPower) {
        memory->write(humanoid.Addr + Offsets::Humanoid::JumpPower, newPower);
        memory->write(humanoid.Addr + 0x1AC, newPower);
    }
}
