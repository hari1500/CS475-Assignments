CC=g++

OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib 
CPPFLAGS=-I/usr/local/include -I./

BIN=./bin/a1model
SRCS=src/a1model.cpp src/gl_framework.cpp src/shader_util.cpp 
INCLUDES=src/gl_framework.hpp src/shader_util.hpp src/a1model.hpp

all: build $(BIN)

$(BIN): $(SRCS) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(LIBS)

build:
	@mkdir -p ./bin

clean:
	rm -f *~ *.o
	rm -rf ./bin
