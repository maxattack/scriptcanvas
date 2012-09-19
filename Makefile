
# tolua++ -o binding.cpp -H binding.h bubbles.tolua

BIN = bubble
CC = clang
CXXC = clang++
CFLAGS = -Os -g -fno-common -I/usr/local/include -Wall -Werror -DDEBUG=1 -Wno-unused-variable
CXXFLAGS = -fno-exceptions -fno-rtti -std=c++11 -stdlib=libc++
LFLAGS = -L/usr/local/lib -lstdc++ -framework OpenGL -framework Cocoa -lglfw -llua

#CFLAGS += -DNO_DAG_SORT

OBJS = \
	src/main.o \
	src/CircleSystem.o \
	src/InputSystem.o \
	src/RenderSystem.o \
	src/SceneSystem.o \
	src/binding.o \
	src/math/Curves.o \
	src/tolua_event.o \
	src/tolua_is.o \
	src/tolua_map.o \
	src/tolua_push.o \
	src/tolua_to.o \


$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(LFLAGS)
	
%.o : %.cpp
	$(CXXC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN) src/main.lua
	./bubble main.lua

clean:
	rm -f $(OBJS) $(BIN)