ifdef ComSpec
	PATHSEP2=\\
	RM =del /s
    EXT=.exe
	config=configure_win
else
	PATHSEP2=/
	RM =rm -rf
	config=configure_linux
    EXT=
endif

CC=g++
CFLAGS=-W -Wall -ansi -pedantic
PATH_SRC=
PATH_DST=..$(PATHSEP2)bin$(PATHSEP2)
LDFLAGS=
EXEC=cavendish

SRC=$(wildcard $(PATH_SRC)*.cpp)
OBJS=$(addprefix $(PATH_DST), $(SRC:.cpp=.o ))


all: $(PATH_DST)$(EXEC) clean launch

$(PATH_DST)$(EXEC): $(OBJS)
	$(CC) $(PATH_DST)$(OBJS) -o $@

$(OBJS): $(SRC)
	echo $(SRC)
	$(CC) $(CFLAGS) -c $< -o $(PATH_DST)$@

configure_win:
	@echo off
	if not exist $(PATH_DST) md  $(PATH_DST)

configure_linux:
	mkdir -p $(PATH_DST)

launch:
	$(PATH_DST)$(EXEC)$(EXT)

clean:
	$(RM) $(PATH_DST)*.o

mrproper: clean
	$(RM) $(PATH_DST)$(EXEC)
