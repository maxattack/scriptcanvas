/*
** Vector math library
** See Copyright Notice in lua.h
*/


#include <stdlib.h>
#include <math.h>

#define lveclib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int vec_ctor (lua_State *L) {
  float x = (float)lua_tonumber(L, 2);
  float y = (float)lua_tonumber(L, 3);
  lua_pushvec(L, x, y);
  return 1;
}

static int vec_new (lua_State *L) {
  float x = (float)lua_tonumber(L, 1);
  float y = (float)lua_tonumber(L, 2);
  lua_pushvec(L, x, y);
  return 1;
}

static int vec_cmul (lua_State *L) {
  lua_Vector v1 = luaL_checkvec(L, 1);
  lua_Vector v2 = luaL_checkvec(L, 2);
  lua_pushvec(L, v1.x*v2.x - v1.y*v2.y, v1.x*v2.y + v1.y * v2.x);
  return 1;
}

static int vec_dot (lua_State *L) {
  lua_Vector v1 = luaL_checkvec(L, 1);
  lua_Vector v2 = luaL_checkvec(L, 2);
  lua_pushnumber(L, v1.x*v2.x + v1.y*v2.y);
  return 1;
}

static int vec_cross (lua_State *L) {
  lua_Vector v1 = luaL_checkvec(L, 1);
  lua_Vector v2 = luaL_checkvec(L, 2);
  lua_pushnumber(L, v1.x * v2.y - v2.x * v1.y);
  return 1;
}

static int vec_length_sq (lua_State *L) {
  lua_Vector v = luaL_checkvec(L, 1);
  lua_pushnumber(L, v.x*v.x + v.y*v.y);
  return 1;
}

static int vec_length (lua_State *L) {
  lua_Vector v = luaL_checkvec(L, 1);
  lua_pushnumber(L, sqrtf(v.x*v.x + v.y*v.y));
  return 1;
}

static int vec_normalize (lua_State *L) {
  lua_Vector v = luaL_checkvec(L, 1);
  float s = 1.0f / sqrtf(v.x*v.x + v.y*v.y);
  lua_pushvec(L, v.x*s, v.y*s);
  return 1;
}

static const luaL_Reg veclib[] = {
  {"new",        vec_new},
  {"cmul",       vec_cmul},
  {"dot",        vec_dot},
  {"cross",      vec_cross},
  {"length",     vec_length},
  {"lengthsq",  vec_length_sq},
  {"normalize",  vec_normalize},
  {NULL, NULL}
};

static void createmetatable (lua_State *L) {
   lua_createtable(L, 0, 1);
   lua_pushcfunction(L, vec_ctor);
   lua_setfield(L, -2, "__call");
   lua_setmetatable(L, -2);
}

/*
** Open veclib
*/
LUAMOD_API int luaopen_vec (lua_State *L) {
  luaL_newlib(L, veclib);
  createmetatable(L);
  lua_pushvec(L, 0, 0);
  lua_setfield(L, -2, "zero");
  lua_pushvec(L, 1, 1);
  lua_setfield(L, -2, "one");
  lua_pushvec(L, 1, 0);
  lua_setfield(L, -2, "unit_x");
  lua_pushvec(L, 0, 1);
  lua_setfield(L, -2, "unit_y");
  return 1;
}
