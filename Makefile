

BIN = bubble
CC = clang
CXXC = clang++
CFLAGS = -fno-common -I/usr/local/include -Wall -Werror -Wno-unused-variable $(OFLAG)
CXXFLAGS = -fno-exceptions -fno-rtti -std=c++11 -stdlib=libc++
LFLAGS = -L/usr/local/lib -lstdc++ -framework OpenGL -framework Cocoa -lglfw $(OFLAG)

CFLAGS += -DLUA_USE_POSIX
#CFLAGS += -DNO_DAG_SORT
#CFLAGS += -DNO_SIMD

ifeq ($(PRODUCTION),1)
CFLAGS += -Os
LFLAGS += -Os
CFLAGS += -DTOLUA_RELEASE
else
CFLAGS += -DDEBUG
CFLAGS += -g
CFLAGS += -Os
LFLAGS += -Os
endif

OBJS = \
	src/CircleManager.o \
	src/InputSystem.o \
	src/RenderSystem.o \
	src/SceneSystem.o \
	src/SplineManager.o \
	src/Math.o \
	src/binding.o \
	src/main.o \
	src/tolua_event.o \
	src/tolua_is.o \
	src/tolua_map.o \
	src/tolua_push.o \
	src/tolua_to.o \
	src/lua/lapi.o \
	src/lua/lauxlib.o \
	src/lua/lbaselib.o \
	src/lua/lcode.o \
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
	src/lua/lvm.o \
	src/lua/lzio.o \
	src/lua/print.o \


$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(LFLAGS)

tools/tolua++: tolua/*.c src/lua/*.c src/tolua_*.c
	$(CC) tolua/*.c src/lua/*.c src/tolua_*.c -o tools/tolua++

src/binding.cpp src/binding.h :  tools/tolua++ src/binding.pkg
	tools/tolua++ -o src/binding.cpp -H src/binding.h src/binding.pkg

%.o : %.cpp src/binding.h src/**.h
	$(CXXC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c src/**.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./bubble main.lua

clean:
	rm -f $(OBJS) $(BIN) tools/tolua++ src/binding.cpp src/binding.h
