PROG=ProtocolTest
OBJS=ProtocolTest.o Protocol.o

CXX=g++

CXXFLAGS=-std=c++17 -g

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS)

Protocol.o: Protocol.cc Protocol.h
ProtocolTest.o: ProtocolTest.cc Protocol.h

clean:
	-rm $(PROG)
	-rm $(OBJS)

.PHONY: all clean
