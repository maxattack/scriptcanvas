/*
** Lua binding: bubbles
** Generated automatically by tolua++-1.0.92 on Mon Aug 20 00:25:54 2012.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_bubbles_open (lua_State* tolua_S);

#include "Script.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* function: SceneSystem::NodeCount */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_nodeCount00
static int tolua_bubbles_SceneSystem_nodeCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   int tolua_ret = (int)  SceneSystem::NodeCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'nodeCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::CreateNode */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_createNode00
static int tolua_bubbles_SceneSystem_createNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int parent = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   unsigned int tolua_ret = ( unsigned int)  SceneSystem::CreateNode(parent);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createNode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::AttachNode */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_attach00
static int tolua_bubbles_SceneSystem_attach00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int parent = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int child = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   SceneSystem::AttachNode(parent,child);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'attach'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::DetachNode */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_detach00
static int tolua_bubbles_SceneSystem_detach00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int child = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   SceneSystem::DetachNode(child);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detach'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::Parent */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_parentOf00
static int tolua_bubbles_SceneSystem_parentOf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   unsigned int tolua_ret = ( unsigned int)  SceneSystem::Parent(node);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'parentOf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::AddComponent */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_addComponent00
static int tolua_bubbles_SceneSystem_addComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int componentType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   SceneSystem::AddComponent(node,componentType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::HasComponent */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_hasComponent00
static int tolua_bubbles_SceneSystem_hasComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int componentType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   bool tolua_ret = (bool)  SceneSystem::HasComponent(node,componentType);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'hasComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::RemoveComponent */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_removeComponent00
static int tolua_bubbles_SceneSystem_removeComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int componentType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   SceneSystem::RemoveComponent(node,componentType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::DestroyNode */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_destroyNode00
static int tolua_bubbles_SceneSystem_destroyNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   SceneSystem::DestroyNode(node);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyNode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::GetPosition */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_positionOf00
static int tolua_bubbles_SceneSystem_positionOf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  float x = ((float)  tolua_tonumber(tolua_S,2,0));
  float y = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   SceneSystem::GetPosition(node,&x,&y);
   tolua_pushnumber(tolua_S,(lua_Number)x);
   tolua_pushnumber(tolua_S,(lua_Number)y);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'positionOf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::SetPosition */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_setPosition00
static int tolua_bubbles_SceneSystem_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  float x = ((float)  tolua_tonumber(tolua_S,2,0));
  float y = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   SceneSystem::SetPosition(node,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::GetRotation */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_rotationOf00
static int tolua_bubbles_SceneSystem_rotationOf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   float tolua_ret = (float)  SceneSystem::GetRotation(node);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rotationOf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SceneSystem::SetRotation */
#ifndef TOLUA_DISABLE_tolua_bubbles_SceneSystem_setRotation00
static int tolua_bubbles_SceneSystem_setRotation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  float degrees = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   SceneSystem::SetRotation(node,degrees);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setRotation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::CreateMaterial */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_createMaterial00
static int tolua_bubbles_CircleSystem_createMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float r = ((float)  tolua_tonumber(tolua_S,1,1));
  float g = ((float)  tolua_tonumber(tolua_S,2,1));
  float b = ((float)  tolua_tonumber(tolua_S,3,1));
  {
   unsigned int tolua_ret = ( unsigned int)  CircleSystem::CreateMaterial(r,g,b);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::SetMaterial */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_setMaterial00
static int tolua_bubbles_CircleSystem_setMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int mat = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   CircleSystem::SetMaterial(node,mat);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::GetMaterial */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_material00
static int tolua_bubbles_CircleSystem_material00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   unsigned int tolua_ret = ( unsigned int)  CircleSystem::GetMaterial(node);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'material'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::SetMaterialColor */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_setMaterialColor00
static int tolua_bubbles_CircleSystem_setMaterialColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int mat = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  float r = ((float)  tolua_tonumber(tolua_S,2,0));
  float g = ((float)  tolua_tonumber(tolua_S,3,0));
  float b = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   CircleSystem::SetMaterialColor(mat,r,g,b);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setMaterialColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::DestroyMaterial */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_destroyMaterial00
static int tolua_bubbles_CircleSystem_destroyMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int matId = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   CircleSystem::DestroyMaterial(matId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::CreateGeometry */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_createGeometry00
static int tolua_bubbles_CircleSystem_createGeometry00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float radius = ((float)  tolua_tonumber(tolua_S,1,1));
  {
   unsigned int tolua_ret = ( unsigned int)  CircleSystem::CreateGeometry(radius);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createGeometry'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::SetGeometry */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_setGeometry00
static int tolua_bubbles_CircleSystem_setGeometry00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int geom = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   CircleSystem::SetGeometry(node,geom);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setGeometry'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::GetGeometry */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_geometry00
static int tolua_bubbles_CircleSystem_geometry00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int node = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   unsigned int tolua_ret = ( unsigned int)  CircleSystem::GetGeometry(node);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'geometry'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::SetGeometryRadius */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_setGeometryRadius00
static int tolua_bubbles_CircleSystem_setGeometryRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int geomId = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  float radius = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   CircleSystem::SetGeometryRadius(geomId,radius);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setGeometryRadius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CircleSystem::DestroyGeometry */
#ifndef TOLUA_DISABLE_tolua_bubbles_CircleSystem_destroyGeometry00
static int tolua_bubbles_CircleSystem_destroyGeometry00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int geomId = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   CircleSystem::DestroyGeometry(geomId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyGeometry'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: InputSystem::GetMousePosition */
#ifndef TOLUA_DISABLE_tolua_bubbles_InputSystem_mousePosition00
static int tolua_bubbles_InputSystem_mousePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int x = ((int)  tolua_tonumber(tolua_S,1,0));
  int y = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   InputSystem::GetMousePosition(&x,&y);
   tolua_pushnumber(tolua_S,(lua_Number)x);
   tolua_pushnumber(tolua_S,(lua_Number)y);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'mousePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: InputSystem::GetTime */
#ifndef TOLUA_DISABLE_tolua_bubbles_InputSystem_seconds00
static int tolua_bubbles_InputSystem_seconds00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   double tolua_ret = (double)  InputSystem::GetTime();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'seconds'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_bubbles_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"SceneSystem",0);
  tolua_beginmodule(tolua_S,"SceneSystem");
   tolua_function(tolua_S,"nodeCount",tolua_bubbles_SceneSystem_nodeCount00);
   tolua_function(tolua_S,"createNode",tolua_bubbles_SceneSystem_createNode00);
   tolua_function(tolua_S,"attach",tolua_bubbles_SceneSystem_attach00);
   tolua_function(tolua_S,"detach",tolua_bubbles_SceneSystem_detach00);
   tolua_function(tolua_S,"parentOf",tolua_bubbles_SceneSystem_parentOf00);
   tolua_function(tolua_S,"addComponent",tolua_bubbles_SceneSystem_addComponent00);
   tolua_function(tolua_S,"hasComponent",tolua_bubbles_SceneSystem_hasComponent00);
   tolua_function(tolua_S,"removeComponent",tolua_bubbles_SceneSystem_removeComponent00);
   tolua_function(tolua_S,"destroyNode",tolua_bubbles_SceneSystem_destroyNode00);
   tolua_function(tolua_S,"positionOf",tolua_bubbles_SceneSystem_positionOf00);
   tolua_function(tolua_S,"setPosition",tolua_bubbles_SceneSystem_setPosition00);
   tolua_function(tolua_S,"rotationOf",tolua_bubbles_SceneSystem_rotationOf00);
   tolua_function(tolua_S,"setRotation",tolua_bubbles_SceneSystem_setRotation00);
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"CircleSystem",0);
  tolua_beginmodule(tolua_S,"CircleSystem");
   tolua_function(tolua_S,"createMaterial",tolua_bubbles_CircleSystem_createMaterial00);
   tolua_function(tolua_S,"setMaterial",tolua_bubbles_CircleSystem_setMaterial00);
   tolua_function(tolua_S,"material",tolua_bubbles_CircleSystem_material00);
   tolua_function(tolua_S,"setMaterialColor",tolua_bubbles_CircleSystem_setMaterialColor00);
   tolua_function(tolua_S,"destroyMaterial",tolua_bubbles_CircleSystem_destroyMaterial00);
   tolua_function(tolua_S,"createGeometry",tolua_bubbles_CircleSystem_createGeometry00);
   tolua_function(tolua_S,"setGeometry",tolua_bubbles_CircleSystem_setGeometry00);
   tolua_function(tolua_S,"geometry",tolua_bubbles_CircleSystem_geometry00);
   tolua_function(tolua_S,"setGeometryRadius",tolua_bubbles_CircleSystem_setGeometryRadius00);
   tolua_function(tolua_S,"destroyGeometry",tolua_bubbles_CircleSystem_destroyGeometry00);
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"InputSystem",0);
  tolua_beginmodule(tolua_S,"InputSystem");
   tolua_function(tolua_S,"mousePosition",tolua_bubbles_InputSystem_mousePosition00);
   tolua_function(tolua_S,"seconds",tolua_bubbles_InputSystem_seconds00);
  tolua_endmodule(tolua_S);

  { /* begin embedded lua code */
   int top = lua_gettop(tolua_S);
   static const unsigned char B[] = {
    10,115, 99,101,110,101, 32, 61, 32, 83, 99,101,110,101, 83,
    121,115,116,101,109, 10, 99,105,114, 99,108,101, 32, 61, 32,
     67,105,114, 99,108,101, 83,121,115,116,101,109, 10,105,110,
    112,117,116, 32, 61, 32, 73,110,112,117,116, 83,121,115,116,
    101,109, 10, 99,111,109,112,111,110,101,110,116, 32, 61, 32,
    123, 10, 99,105,114, 99,108,101, 32, 61, 32, 48, 10,125, 10,
    102,117,110, 99,116,105,111,110, 32,105,109,112,111,114,116,
     40,110,115, 41, 10,102,111,114, 32,107,101,121, 44,118, 97,
    108,117,101, 32,105,110, 32,112, 97,105,114,115, 40,110,115,
     41, 32,100,111, 10, 95, 71, 91,107,101,121, 93, 32, 61, 32,
    118, 97,108,117,101, 10,101,110,100, 10,101,110,100, 45, 45,
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,32
   };
   tolua_dobuffer(tolua_S,(char*)B,sizeof(B),"tolua: embedded Lua code 1");
   lua_settop(tolua_S, top);
  } /* end of embedded lua code */

 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_bubbles (lua_State* tolua_S) {
 return tolua_bubbles_open(tolua_S);
};
#endif

