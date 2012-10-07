This version of Lua has been modified to support simple 2D vectors on the stack (via the new "vec" auxlib).

The implementation was inspired by lua-vec (http://code.google.com/p/lua-vec/) but doesn't implement 4-vectors (to avoid the storage problem), and targets a newer version of Lua.

A quick search for "MAXATTACK" reveals every part of the code which has been modified this way.