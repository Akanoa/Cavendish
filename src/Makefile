ifdef ComSpec
	PATH_SRC=.
	PATH_DST=..\\bin\\
	RM =rm -rf
	EXT=.exe
else
	PATH_SRC=
	PATH_DST=../bin/
	RM =rm -rf
	EXT=
endif

CC=g++
CFLAGS=-W -Wall -ansi -pedantic -g
LDFLAGS=
EXEC=cavendish

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(PATH_DST), $(SRC:.cpp=.o))

all: $(PATH_DST)$(EXEC)

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
