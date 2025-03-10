COMPILER =gcc

PROGRAM_OPT =args

TARGET_SRV =bin/server
TARGET_CLI =bin/client

SRV_INC_DIRS =-Iinc -Iinc/lib -Iinc/srv -Iinc/srv/args 
CLI_INC_DIRS =-Iinc -Iinc/lib -Iinc/cli -Iinc/cli/args

SRV_LIBS =-pthread -lqueue -Lsrc/lib  #-lm
CLI_LIBS =-pthread

CFLAGS =-ggdb -std=c11 -Wall -Wextra -fno-short-enums -Wpedantic -pedantic -pedantic-errors -Wmissing-declarations -Wmissing-include-dirs -Wundef -Wfloat-equal -ggdb -D_POSIX_C_SOURCE=200809L# -Werror -pg

SRC_SRV =$(wildcard src/srv/*.c)
OBJ_SRV   =$(SRC_SRV:src/srv/%.c=obj/srv/%.o)
SERVER_OBJECTS =$(OBJ_SRV) obj/common.o obj/srv/args/$(PROGRAM_OPT).o

SRC_CLI =$(wildcard src/cli/*.c) 
OBJ_CLI   =$(SRC_CLI:src/cli/%.c=obj/cli/%.o) 
CLIENT_OBJECTS =$(OBJ_CLI) obj/common.o obj/cli/args/$(PROGRAM_OPT).o

.PHONY: all run default clean 

all: obj/srv/args/$(PROGRAM_OPT).o obj/cli/args/$(PROGRAM_OPT).o $(TARGET_SRV) $(TARGET_CLI)

run: clean rmlogs default
	./$(TARGET_SRV) --port 8080 

runld: clean rmlogs default
	LD_LIBRARY_PATH=./src/lib ./$(TARGET_SRV) --port 8080 

rmlogs:
	rm -f logs/srv/*
	rm -f logs/cli/*

default: $(TARGET_SRV) $(TARGET_CLI)

clean:
	rm -f inc/srv/args/$(PROGRAM_OPT).h 
	rm -f src/srv/args/$(PROGRAM_OPT).c
	rm -f obj/srv/args/$(PROGRAM_OPT).o
	rm -f inc/cli/args/$(PROGRAM_OPT).h 
	rm -f src/cli/args/$(PROGRAM_OPT).c
	rm -f obj/cli/args/$(PROGRAM_OPT).o
	rm -f obj/srv/*.o
	rm -f obj/cli/*.o
	rm -f obj/*.o
	rm -f bin/*

# Dependencies
# Server
obj/srv/main.o: src/srv/main.c inc/srv/main.h inc/common.h inc/srv/args/$(PROGRAM_OPT).h inc/srv/server_socket.h inc/srv/server_threads.h inc/srv/server_queues.h 
obj/srv/args/$(PROGRAM_OPT).o: src/srv/args/$(PROGRAM_OPT).c inc/srv/args/$(PROGRAM_OPT).h src/srv/args/$(PROGRAM_OPT).ggo
obj/srv/server_socket.o:  src/srv/server_socket.c  inc/srv/server_socket.h  inc/common.h
obj/srv/server_threads.o: src/srv/server_threads.c inc/srv/server_threads.h inc/common.h 
obj/srv/server_queues.o:  src/srv/server_queues.c  inc/srv/server_queues.h  inc/common.h
# Client
obj/cli/main.o: src/cli/main.c inc/cli/main.h inc/common.h inc/cli/args/$(PROGRAM_OPT).h inc/cli/client_socket.h inc/cli/client_threads.h
obj/cli/args/$(PROGRAM_OPT).o: src/cli/args/$(PROGRAM_OPT).c inc/cli/args/$(PROGRAM_OPT).h src/cli/args/$(PROGRAM_OPT).ggo
obj/cli/client_socket.o: src/cli/client_socket.c inc/cli/client_socket.h inc/common.h
obj/cli/client_threads.o: src/cli/client_threads.c inc/cli/client_threads.h inc/common.h
# Common
obj/common.o: src/common.c inc/common.h

# Generates command line arguments code from gengetopt .ggo configuration file (gengetopt)
# Server
src/srv/args/$(PROGRAM_OPT).c inc/srv/args/$(PROGRAM_OPT).h: src/srv/args/$(PROGRAM_OPT).ggo
	gengetopt --input=src/srv/args/$(PROGRAM_OPT).ggo --file-name=$(PROGRAM_OPT) --header-output-dir=inc/srv/args --src-output-dir=src/srv/args
# Client
src/cli/args/$(PROGRAM_OPT).c inc/cli/args/$(PROGRAM_OPT).h: src/cli/args/$(PROGRAM_OPT).ggo
	gengetopt --input=src/cli/args/$(PROGRAM_OPT).ggo --file-name=$(PROGRAM_OPT) --header-output-dir=inc/cli/args --src-output-dir=src/cli/args

# Generate gengetopt .o files with no warnings
# Server
obj/srv/args/$(PROGRAM_OPT).o: src/srv/args/$(PROGRAM_OPT).c inc/srv/args/$(PROGRAM_OPT).h
	$(COMPILER) -ggdb -std=c11 -pedantic -c $< -o $@ $(SRV_INC_DIRS)
# Client
obj/cli/args/$(PROGRAM_OPT).o: src/cli/args/$(PROGRAM_OPT).c inc/cli/args/$(PROGRAM_OPT).h
	$(COMPILER) -ggdb -std=c11 -pedantic -c $< -o $@ $(CLI_INC_DIRS)


obj/common.o: src/common.c inc/common.h
	$(COMPILER) $(CFLAGS) -c $< -o $@ -Iinc

# Generate .o files from every .c file in src/srv
$(OBJ_SRV): obj/srv/%.o: src/srv/%.c
	$(COMPILER) $(CFLAGS) -c $< -o $@ $(SRV_INC_DIRS)

# Compile Server
$(TARGET_SRV): $(SERVER_OBJECTS) 
	$(COMPILER) -g -o $@ $^ $(SRV_LIBS) 

# Generate .o files from every .c file in src/cli
$(OBJ_CLI): obj/cli/%.o: src/cli/%.c
	$(COMPILER) $(CFLAGS) -c $< -o $@ $(CLI_INC_DIRS)

# Compile Client
$(TARGET_CLI): $(CLIENT_OBJECTS) 
	$(COMPILER) -o $@ $^ $(CLI_LIBS)