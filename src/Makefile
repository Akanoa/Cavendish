ifdef ComSpec
	PATHSEP2=\\
	RM =rm -rf
    EXT=.exe
	config=configure_win
else
	PATHSEP2=/
	RM =rm -rf
	config=configure_linux
    EXT=
endif

CC=g++
CFLAGS=-W -Wall -ansi -pedantic -g
PATH_SRC=.
PATH_DST=..$(PATHSEP2)bin$(PATHSEP2)
LDFLAGS=
EXEC=cavendish

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(PATH_DST), $(SRC:.cpp=.o))

all: $(PATH_DST)$(EXEC) launch clean

$(PATH_DST)$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(PATH_DST)%.o : $(PATH_SRC)%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

launch:
	$(PATH_DST)$(EXEC)$(EXT)

clean:
	$(RM) $(PATH_DST)*.o

mrproper: clean
	$(RM) $(PATH_DST)$(EXEC)
