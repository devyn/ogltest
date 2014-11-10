CXX      = clang++
CXXFLAGS =
LDFLAGS  = -lGL -lGLEW -lglfw

OBJECTS  = ogltest.o shaders.o image.o

%.o: %.cc
	${CXX} -c $< -o $@ ${CXXFLAGS}

ogltest: ${OBJECTS}
	${CXX} $^ -o $@ ${LDFLAGS}

all: ogltest

clean:
	rm -f ${OBJECTS}
	rm -f ogltest

.PHONY: clean
