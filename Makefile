CC=g++
CFLAGS=-Wall -g
BIN=lcwftpd
OBJS=main.o 
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ 
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY:clean
clean:
	rm -f *.o $(BIN)