ARCH=$(shell uname)

LIBS=-lsimavr -lsimavrparts -lelf
CFLAGS=-g

ifeq ($(ARCH),Darwin)
  CC=clang
  CFLAGS += -Wno-deprecated-declarations
  LIBS += -framework GLUT -framework OpenGL
else ifeq ($(ARCH), Linux)
  CC=gcc
else
  $(error unsupported architecture)
endif

all: rotenc-sim

rotenc-sim: rotenc-sim.c
	$(CC) -I /usr/local/include/simavr $(CFLAGS) -o rotenc-sim rotenc-sim.c $(LIBS)

clean:
	rm rotenc-sim

