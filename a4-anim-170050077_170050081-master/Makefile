CC=g++

OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib 
CPPFLAGS=-I/usr/local/include -I./

BIN0=./bin/a3-model-0
SRCS0=src/model0.cpp src/gl_framework0.cpp src/shader_util.cpp src/texture.cpp
INCLUDES0=src/stb_image.h src/gl_framework.hpp src/shader_util.hpp src/model0.hpp src/texture.hpp

BIN1=./bin/a3-model-1
SRCS1=src/model1.cpp src/gl_framework1.cpp src/shader_util.cpp src/texture.cpp
INCLUDES1=src/stb_image.h src/gl_framework.hpp src/shader_util.hpp src/model1.hpp src/texture.hpp

BIN2=./bin/a3-model-2
SRCS2=src/model2.cpp src/gl_framework2.cpp src/shader_util.cpp src/texture.cpp src/hierarchy_node.cpp
INCLUDES2=src/stb_image.h src/gl_framework.hpp src/shader_util.hpp src/model2.hpp src/texture.hpp src/hierarchy_node.hpp

BIN_S=./bin/a3-scene
SRCS_S=src/scene.cpp src/gl_framework_scene.cpp src/shader_util.cpp src/texture.cpp src/hierarchy_node.cpp
INCLUDES_S=src/stb_image.h src/gl_framework.hpp src/shader_util.hpp src/scene.hpp src/texture.hpp src/hierarchy_node.hpp

BIN_B=./bin/a4-bezier
SRCS_B=src/bezier.cpp src/gl_fw_bezier.cpp src/shader_util.cpp
INCLUDES_B=src/gl_fw_bezier.hpp src/shader_util.hpp src/bezier.hpp

all: build $(BIN0) $(BIN1) $(BIN2) $(BIN_S) $(BIN_B) 

$(BIN0): $(SRCS0) $(INCLUDES0)
	g++ $(CPPFLAGS) $(SRCS0) -o $(BIN0) $(LDFLAGS) $(LIBS)

$(BIN1): $(SRCS1) $(INCLUDES1)
	g++ $(CPPFLAGS) $(SRCS1) -o $(BIN1) $(LDFLAGS) $(LIBS)

$(BIN2): $(SRCS2) $(INCLUDES2)
	g++ $(CPPFLAGS) $(SRCS2) -o $(BIN2) $(LDFLAGS) $(LIBS)

$(BIN_S): $(SRCS_S) $(INCLUDES_S)
	g++ $(CPPFLAGS) $(SRCS_S) -o $(BIN_S) $(LDFLAGS) $(LIBS)

$(BIN_B): $(SRCS_B) $(INCLUDES_B)
	g++ $(CPPFLAGS) $(SRCS_B) -o $(BIN_B) $(LDFLAGS) $(LIBS)


build:
	@mkdir -p ./bin

clean:
	rm -f *~ *.o
	rm -rf ./bin