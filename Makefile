CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Iinclude -g -fsanitize=address,undefined

SRC = src/cartas.c src/baraja_espanola.c src/mus.c
TESTS = build/test_cartas build/test_baraja_espanola build/test_mus build/test_partida

.PHONY: test clean

test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

build/%: tests/%.c $(SRC) include/*.h tests/utiles_test.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< $(SRC)

clean:
	rm -rf build
