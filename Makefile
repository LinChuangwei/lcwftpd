#目标
TARGET=lcwftpd
SOURCES=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))
CXX=g++
CFLAGS=-Wall -g -std=c++11 -lcrypt


$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $^ -o $@ 
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f *.o 