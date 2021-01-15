# File: Standard Makefile

ROOTNAME=word_blast
HW=
FOPTION=_main
RUNOPTIONS=WarAndPeace.txt 4
CC=gcc
CFLAGS= -g -I.
LIBS =pthread
DEPS = 
OBJ = $(ROOTNAME)$(HW)$(FOPTION).o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(ROOTNAME)$(HW)$(FOPTION): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -l $(LIBS)

clean:
	rm *.o $(ROOTNAME)$(HW)$(FOPTION)

run: $(ROOTNAME)$(HW)$(FOPTION)
	./$(ROOTNAME)$(HW)$(FOPTION) $(RUNOPTIONS)
