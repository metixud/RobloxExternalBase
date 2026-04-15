#pragma once
#include "../../../src/sdk/sdk.h"

namespace Globals {
    inline RBX::RbxInstance dataModel(0);
    inline RBX::RenderEngine renderEngine(0);
    inline RBX::RbxInstance workspace(0);
    inline RBX::RbxInstance players(0);
    inline bool running = true;
    inline RBX::RbxInstance camera(0);
    inline RBX::RbxInstance localPlayer(0);
}
