#pragma once

namespace variables {
    inline bool menuOpen = false;
    inline int selectedTab = 0;

    namespace Aimbot {
        inline bool enabled = false;
        inline bool showFOV = false;
        inline float fovRadius = 100.0f;
        inline float smoothing = 5.0f;
        inline int aimTarget = 0;
        inline int aimMethod = 0;
        inline int aimbotKey = 2;
    }

    namespace ESP {
        inline bool enabled = false;
        inline bool boxes = false;
        inline bool names = false;
        inline bool distance = false;
        inline bool healthBar = false;
        inline bool snaplines = false;
        inline int snaplinesOrigin = 3;
        inline int snaplinesDestination = 0;
        inline int snaplinesStyle = 0;
        inline float snaplinesThickness = 1.0f;
        inline bool snaplinesOutline = true;
    }

    namespace Local {
        inline bool speedEnabled = false;
        inline float walkSpeed = 16.0f;
        inline bool jumpEnabled = false;
        inline float jumpPower = 50.0f;
        inline bool desyncEnabled = false;
    }
}
