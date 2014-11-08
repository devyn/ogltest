CXX      = clang++
CXXFLAGS =
LDFLAGS  = -lGL -lGLEW -lglfw

%.o: %.cc
	${CXX} -c $< -o $@ ${CXXFLAGS}

ogltest: ogltest.o
	${CXX} $< -o $@ ${LDFLAGS}

all: ogltest

clean:
	rm -f ogltest.o
	rm -f ogltest

.PHONY: clean
