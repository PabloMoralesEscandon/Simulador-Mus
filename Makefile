CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Iinclude -g -fsanitize=address,undefined
LDLIBS = -lm

SRC = src/cartas.c src/baraja_espanola.c src/mus.c src/mus_sim.c src/mus_log.c
TESTS = build/test_cartas build/test_baraja_espanola build/test_mus build/test_partida build/test_mus_sim
EXPERIMENTOS = build/experimento1

.PHONY: all test experimentos clean

all: partida_sim.out test

test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

experimentos: $(EXPERIMENTOS)

partida_sim.out: programas/partida_sim.c $(SRC) include/*.h
	$(CC) $(CFLAGS) -o $@ $< $(SRC) $(LDLIBS)

build/experimento1: experimentos/experimento1.c $(SRC) include/*.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< $(SRC) $(LDLIBS)

build/%: tests/%.c $(SRC) include/*.h tests/utiles_test.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< $(SRC) $(LDLIBS)

clean:
	rm -rf build partida_sim.out
