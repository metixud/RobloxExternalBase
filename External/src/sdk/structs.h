#include <cstdint>
#include <string>
#include "math.h"

using Vector2 = rbx::vector2_t;
using Vector3 = rbx::vector3_t;
using Matrix3x3 = rbx::matrix3_t;
using UDim2 = rbx::udim2_t;
using ViewMatrix_t = rbx::matrix4_t;

// trust ud!
namespace Structs {
    inline std::string ClientVersion = "version-9d412f44a6fe4081";
  struct Humanoid {
        char pad_0[32];
        int HumanoidStateID;
        char pad_1[252];
        uintptr_t SeatPart;
        char pad_2[8];
        uintptr_t MoveToPart;
        char pad_3[8];
        Vector3 CameraOffset;
        char pad_4[12];
        Vector3 MoveDirection;
        Vector3 TargetPoint;
        char pad_5[12];
        Vector3 MoveToPoint;
        char pad_6[4];
        int DisplayDistanceType;
        int FloorMaterial;
        float Health;
        float HealthDisplayDistance;
        int HealthDisplayType;
        float HipHeight;
        char pad_7[8];
        float JumpHeight;
        float JumpPower;
        float MaxHealth;
        float MaxSlopeAngle;
        float NameDisplayDistance;
        int NameOcclusion;
        char pad_8[4];
        int RigType;
        char pad_9[8];
        float Walkspeed;
        bool AutoJumpEnabled;
        bool AutoRotate;
        bool AutomaticScalingEnabled;
        bool BreakJointsOnDeath;
        bool EvaluateStateMachine;
        bool Jump;
        char pad_10[1];
        bool PlatformStand;
        bool Sit;
        bool RequiresNeck;
        char pad_11[1];
        bool UseJumpPower;
        char pad_12[472];
        float WalkspeedCheck;
        char pad_13[80];
        double WalkTimer;
        char pad_14[96];
        uintptr_t HumanoidRootPart;
        char pad_15[1048];
        int HumanoidState;
        char pad_16[123];
        bool IsWalking;
    }; // sizeof = 2328
}
