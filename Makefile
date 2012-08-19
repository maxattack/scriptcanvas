
# tolua++ -o binding.cpp -H binding.h bubbles.tolua

BIN = bubble
CC = clang
CFLAGS = -Os -g -fno-common -I/usr/local/include -Wall -Wno-c++11-extensions -Werror 
CFLAGS += -DDEBUG=1
CXXFLAGS = -fno-exceptions -fno-rtti
LFLAGS = -L/usr/local/lib -lstdc++ -framework OpenGL -framework Cocoa -lglfw -llua

OBJS = \
	src/main.o \
	src/binding.o \
	src/RenderSystem.o \
	src/SceneSystem.o \
	src/tolua_event.o \
	src/tolua_is.o \
	src/tolua_map.o \
	src/tolua_push.o \
	src/tolua_to.o \


$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(LFLAGS)
	
%.o : %.cpp 
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS) $(BIN)