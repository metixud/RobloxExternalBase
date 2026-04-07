#pragma once

namespace variables {
    inline bool menuOpen = false;
    inline int selectedTab = 0;

    namespace ESP {
        inline bool enabled = false;
        inline bool boxes = false;
        inline bool names = false;
        inline bool distance = false;
        inline bool healthBar = false;
    }

    namespace Local {
        inline bool speedEnabled = false;
        inline float walkSpeed = 16.0f;
        inline bool jumpEnabled = false;
        inline float jumpPower = 50.0f;
    }
}
