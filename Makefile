CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Iinclude -g -fsanitize=address,undefined

SRC = src/cartas.c src/baraja_espanola.c src/mus.c
TESTS = build/test_cartas build/test_baraja_espanola build/test_mus build/test_partida
EXPERIMENTOS = build/experimento1

.PHONY: test experimentos clean

test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

experimentos: $(EXPERIMENTOS)

build/experimento1: experimentos/experimento1.c $(SRC) src/mus_sim.c include/*.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< $(SRC) src/mus_sim.c

build/%: tests/%.c $(SRC) include/*.h tests/utiles_test.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< $(SRC)

clean:
	rm -rf build
