IDIR =.
CC=g++
LDLIBS =  -lsfml-graphics -lsfml-window -lsfml-system -lm
CFLAGS=-I$(IDIR) -g -Wextra

LDFLAGS= $(CFLAGS)

ODIR=.
LIBS= $(LDLIBS) $(LDFLAGS)

_DEPS = window.h main_class.h ghost.h pacman.h interface.h keyboard.h srv/cpu.h srv/debug.h srv/list.h srv/semaphore.h srv/system.h srv/thread.h srv/timer.h srv/traits.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o window.o main_class.o ghost.o pacman.o interface.o keyboard.o srv/debug.o srv/system.o srv/thread.o srv/cpu.o srv/semaphore.o srv/timer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -std=c++14 -c -o $@ $< $(CFLAGS) -Wextra

jogo: $(OBJ)
	$(CC) -std=c++14 -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

run:
	./jogo

runV:
	valgrind --leak-check=full --show-leak-kinds=all ./jogo

runG:
	gdb jogo

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ && rm srv/*.o && rm jogo
