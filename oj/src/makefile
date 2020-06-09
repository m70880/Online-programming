LDFLAGS=-L /home/m70880/ThirdPartLibForCpp/el7.x86_64/third_part/lib
DEBUGFLAGS=-g
BIN=../bin/oj_server
CC=g++

.PHONY:$(BIN)
$(BIN):oj_server.cpp
	$(CC) $^ -o $@  $(DEBUGFLAGS) $(LDFLAGS) -lctemplate -ljsoncpp -lpthread -std=c++11

.PHONY:clean
clean:
	rm -f $(BIN)
#
#.PHONY:jout
#jout:oj_server.cpp 
#	g++ $^ -o $@ -lpthread -g -L /home/m70880/ThirdPartLibForCpp/el7.x86_64/third_part/lib -lctemplate  -ljsoncpp -std=c++11
#
#.PHONY:clean
#clean:
#	rm tmp_files/*
#
