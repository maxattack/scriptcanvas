BIN = bubble
CC = clang
CFLAGS = -Os -g -fno-common -I/usr/local/include -Wall -Wno-c++11-extensions -Werror 
CFLAGS += -DDEBUG=1
CXXFLAGS = -fno-exceptions -fno-rtti
LFLAGS = -L/usr/local/lib -lstdc++ -framework OpenGL -framework Cocoa -lglfw

OBJS = \
	src/main.o \
	src/RenderSystem.o \
	src/SceneSystem.o


$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(LFLAGS)
	
%.o : %.cpp 
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJS) $(BIN)