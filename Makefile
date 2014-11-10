CXX      = clang++
CXXFLAGS =
LDFLAGS  = -lGL -lGLEW -lglfw

%.o: %.cc
	${CXX} -c $< -o $@ ${CXXFLAGS}

ogltest: ogltest.o shaders.o
	${CXX} $^ -o $@ ${LDFLAGS}

all: ogltest

clean:
	rm -f ogltest.o
	rm -f shaders.o
	rm -f ogltest

.PHONY: clean
