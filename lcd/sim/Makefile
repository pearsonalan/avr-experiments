ARCH=$(shell uname)

# determine compiler
ifeq ($(ARCH),Darwin)
  CC=clang
else ifeq ($(ARCH), Linux)
  CC=gcc
else
  $(error unsupported architecture)
endif

# OpenGL settings
ifeq ($(ARCH),Darwin)
  OPENGL_LDFLAGS = -framework GLUT -framework OpenGL
else ifeq ($(ARCH), Linux)
  OPENGL_LDFLAGS = -lglut ${shell pkg-config --libs glu}
  OPENGL_CFLAGS	 = ${shell pkg-config --cflags glu}
endif

# other flags
ifeq ($(ARCH),Darwin)
  LDFLAGS = -lsimavr -lelf
else ifeq ($(ARCH), Linux)
  LDFLAGS = -lpthread -lsimavr -lelf
endif

CFLAGS	+= $(OPENGL_LDFLAGS)
LDFLAGS += $(OPENGL_LDFLAGS)

PROG=avr-sim
OBJS=avr-sim.o hd44780_glut.o hd44780.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -g -o $(PROG) $(OBJS)  $(LDFLAGS)

%.o: %.c
	$(CC) -I /usr/local/include/simavr $(CFLAGS) -c -g -o $@ $<

clean:
	rm $(PROG) $(OBJS)
