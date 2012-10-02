

BIN = bubble
CC = clang
CXXC = clang++
CFLAGS = -fno-common -I/usr/local/include -Wall -Werror -Wno-unused-variable $(OFLAG)
CXXFLAGS = -fno-exceptions -fno-rtti -std=c++11 -stdlib=libc++
LFLAGS = -L/usr/local/lib  -stdlib=libc++ -framework OpenGL -framework Cocoa -lglfw $(OFLAG)

CFLAGS += -DLUA_USE_POSIX
#CFLAGS += -DNO_DAG_SORT
#CFLAGS += -DNO_SIMD

ifeq ($(PRODUCTION),1)
CFLAGS += -O4
LFLAGS += -O4
else
CFLAGS += -DDEBUG
CFLAGS += -g
CFLAGS += -Os
LFLAGS += -Os
endif

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
	src/TagSystem.o \
	src/main.o

TOOLS = \
	tools/lua \
	tools/luac

$(BIN): $(OBJS) $(LUA_OBJS)
	$(CXXC) $(OBJS) $(LUA_OBJS) -o $(BIN) $(LFLAGS)

tools: $(TOOLS)

tools/lua: $(LUA_OBJS) src/lua/lua.o
	$(CC) $(LUA_OBJS) src/lua/lua.o $(LFLAGS) -o tools/lua

tools/luac: $(LUA_OBJS) src/lua/luac.o
	$(CC) $(LUA_OBJS) src/lua/luac.o $(LFLAGS) -o tools/luac

src/ScriptSystem_Bind.cpp: tools/write_bindings.py
	python tools/write_bindings.py src/ScriptSystem_Bind.cpp

%.o : %.cpp src/**.h
	$(CXXC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c src/**.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./bubble main.lua

clean:
	rm -f $(OBJS) $(LUA_OBJS) $(BIN) $(TOOLS) src/ScriptSystem_Bind.cpp
