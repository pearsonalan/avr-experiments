ARCH=$(shell uname)

ifeq ($(ARCH),Darwin)
  CC=clang
  LDFLAGS 	+= -framework GLUT -framework OpenGL
else ifeq ($(ARCH), Linux)
  CC=gcc
  CFLAGS	+= ${shell pkg-config --cflags glu}
  LDFLAGS 	+= ${shell pkg-config --libs glu} -lglut
else
  $(error unsupported architecture)
endif

PROG=avr-sim
OBJS=avr-sim.o hd44780_glut.o hd44780.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -g -o $(PROG) $(OBJS) -lpthread -lsimavr -lelf $(LDFLAGS)

hd44780.o: hd44780.c
	$(CC) -I /usr/local/include/simavr $(CFLAGS) -c -g -o hd44780.o hd44780.c

hd44780_glut.o: hd44780_glut.c
	$(CC) -I /usr/local/include/simavr $(CFLAGS) -c -g -o hd44780_glut.o hd44780_glut.c

avr-sim.o: avr-sim.c
	$(CC) -I /usr/local/include/simavr $(CFLAGS) -c -g -o avr-sim.o avr-sim.c 

clean:
	rm $(PROG) $(OBJS)

