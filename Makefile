CXX      = clang++
CXXFLAGS = -std=c++11
LDFLAGS  = -lGL -lGLEW -lglfw

OBJECTS  = ogltest.o shaders.o image.o camera.o obj.o

%.o: %.cc
	${CXX} -c $< -o $@ ${CXXFLAGS}

ogltest: ${OBJECTS}
	${CXX} $^ -o $@ ${LDFLAGS}

all: ogltest

clean:
	rm -f ${OBJECTS}
	rm -f ogltest

.PHONY: clean
