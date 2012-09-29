#pragma once
#include "CommandSystem.h"
#include "InputSystem.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

namespace ScriptSystem {
void Bind(lua_State* L);
void Yield();
}

