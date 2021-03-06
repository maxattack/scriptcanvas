# ScriptCanvas: An Artful, Interactive Canvas
# Copyright (C) 2012 max.kaufmann@gmail.com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Only set up for OSX right now, but easily ammended for windows, linux, etc

BIN = bubble
CC = clang
CXXC = clang++
CFLAGS = -fno-common -Wall -Werror -Wno-unused-variable $(OFLAG)
CXXFLAGS = -fno-exceptions -fno-rtti -std=c++11 -stdlib=libc++
LFLAGS = -L/usr/local/lib  -stdlib=libc++ -framework OpenGL -framework Cocoa -framework IOKit $(OFLAG)

CFLAGS += -DLUA_USE_POSIX
#CFLAGS += -DNO_DAG_SORT
#CFLAGS += -DNO_SIMD

# for glfw :P
CFLAGS += -Wno-deprecated-declarations -Wno-implicit-function-declaration -Isrc/glfw -Isrc/glfw/cocoa

ifeq ($(PRODUCTION),1)
CFLAGS += -O4
LFLAGS += -O4
else
CFLAGS += -DDEBUG
CFLAGS += -g
CFLAGS += -O3
LFLAGS += -O3
endif

GENERATED_HEADERS = \
	src/Shaders.h

GENERATED_SOURCE = \
	src/ScriptSystem_Bind.cpp 

LUA_OBJS = \
	src/lua/lapi.o \
	src/lua/lauxlib.o \
	src/lua/lbaselib.o \
	src/lua/lbitlib.o \
	src/lua/lcode.o \
	src/lua/lcorolib.o \
	src/lua/lctype.o \
	src/lua/ldblib.o \
	src/lua/ldebug.o \
	src/lua/ldo.o \
	src/lua/ldump.o \
	src/lua/lfunc.o \
	src/lua/lgc.o \
	src/lua/linit.o \
	src/lua/liolib.o \
	src/lua/llex.o \
	src/lua/lmathlib.o \
	src/lua/lmem.o \
	src/lua/loadlib.o \
	src/lua/lobject.o \
	src/lua/lopcodes.o \
	src/lua/loslib.o \
	src/lua/lparser.o \
	src/lua/lstate.o \
	src/lua/lstring.o \
	src/lua/lstrlib.o \
	src/lua/ltable.o \
	src/lua/ltablib.o \
	src/lua/ltm.o \
	src/lua/lundump.o \
	src/lua/lvec.o \
	src/lua/lvm.o \
	src/lua/lzio.o

GLFW_OBJS = \
	src/glfw/enable.o \
	src/glfw/fullscreen.o \
	src/glfw/glext.o \
	src/glfw/image.o \
	src/glfw/init.o \
	src/glfw/input.o \
	src/glfw/joystick.o \
	src/glfw/stream.o \
	src/glfw/tga.o \
	src/glfw/thread.o \
	src/glfw/time.o \
	src/glfw/window.o \
	src/glfw/cocoa/cocoa_enable.o \
	src/glfw/cocoa/cocoa_fullscreen.o \
	src/glfw/cocoa/cocoa_glext.o \
	src/glfw/cocoa/cocoa_init.o \
	src/glfw/cocoa/cocoa_joystick.o \
	src/glfw/cocoa/cocoa_thread.o \
	src/glfw/cocoa/cocoa_time.o \
	src/glfw/cocoa/cocoa_window.o

OBJS = \
	src/CircleSystem.o \
	src/CommandSystem.o \
	src/InputSystem.o \
	src/Math.o \
	src/MaterialSystem.o \
	src/NodeSystem.o \
	src/RenderSystem.o \
	src/ScriptSystem_Bind.o \
	src/SplineSystem.o \
	src/StrokeSystem.o \
	src/TagSystem.o \
	src/main.o

TOOLS = \
	tools/lua \
	tools/luac

$(BIN): $(OBJS) $(LUA_OBJS) $(GLFW_OBJS)
	$(CXXC) $(OBJS) $(LUA_OBJS) $(GLFW_OBJS) -o $(BIN) $(LFLAGS)

tools: $(TOOLS)

tools/lua: $(LUA_OBJS) src/lua/lua.o
	$(CC) $(LUA_OBJS) src/lua/lua.o $(LFLAGS) -o tools/lua

tools/luac: $(LUA_OBJS) src/lua/luac.o
	$(CC) $(LUA_OBJS) src/lua/luac.o $(LFLAGS) -o tools/luac

src/ScriptSystem_Bind.cpp: tools/write_lua_bindings.py
	python tools/write_lua_bindings.py src/ScriptSystem_Bind.cpp

src/Shaders.h: tools/write_glsl_bindings.py src/*.glsl
	python tools/write_glsl_bindings.py $@

%.o : %.cpp $(GENERATED_HEADERS)
	$(CXXC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.m
	$(CC) $(CFLAGS) -c $< -o $@	

run: $(BIN)
	./bubble main.lua

cleanroot:
	rm -rf $(OBJS) $(BIN)

clean:
	rm -f $(OBJS) $(LUA_OBJS) $(GLFW_OBJS) $(BIN) $(TOOLS) $(GENERATED_SOURCE) $(GENERATED_HEADERS)
