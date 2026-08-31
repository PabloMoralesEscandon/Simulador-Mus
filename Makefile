CC = gcc
CPPFLAGS ?=
CFLAGS ?= -O2
TEST_CFLAGS ?= -O1 -g -fsanitize=address,undefined
LDFLAGS ?=
LDLIBS ?= -lm

COMMON_CFLAGS = -Iinclude -std=gnu99 -Wall -Wextra -Wshadow -Wstrict-prototypes \
	-Wold-style-definition

SRC = src/cartas.c src/baraja_espanola.c src/mus.c src/mus_sim.c src/mus_log.c
RELEASE_OBJS = $(patsubst src/%.c,build/release/src/%.o,$(SRC))
TEST_OBJS = $(patsubst src/%.c,build/test/src/%.o,$(SRC))

TEST_NAMES = cartas baraja_espanola mus partida mus_sim dos_jugadores
TESTS = $(addprefix build/test_,$(TEST_NAMES))
TEST_MAIN_OBJS = $(addprefix build/test/tests/test_,$(addsuffix .o,$(TEST_NAMES)))
RELEASE_TESTS = $(addprefix build/release_test_,$(TEST_NAMES))
RELEASE_TEST_MAIN_OBJS = \
	$(addprefix build/release/tests/test_,$(addsuffix .o,$(TEST_NAMES)))

EXPERIMENTOS = build/experimento1
RELEASE_MAIN_OBJS = build/release/programas/partida_sim.o \
	build/release/experimentos/experimento1.o
DEPS = $(RELEASE_OBJS:.o=.d) $(TEST_OBJS:.o=.d) \
	$(TEST_MAIN_OBJS:.o=.d) $(RELEASE_TEST_MAIN_OBJS:.o=.d) \
	$(RELEASE_MAIN_OBJS:.o=.d)

.PHONY: all test test-release experimentos clean
.SECONDARY: $(TEST_MAIN_OBJS) $(RELEASE_TEST_MAIN_OBJS)

all: partida_sim.out test

test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

test-release: $(RELEASE_TESTS)
	@for t in $(RELEASE_TESTS); do ./$$t || exit 1; done

experimentos: $(EXPERIMENTOS)

partida_sim.out: build/release/programas/partida_sim.o $(RELEASE_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/experimento1: build/release/experimentos/experimento1.o $(RELEASE_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/test_%: build/test/tests/test_%.o $(TEST_OBJS)
	$(CC) $(TEST_CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/release_test_%: build/release/tests/test_%.o $(RELEASE_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/release/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

build/release/programas/%.o: programas/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

build/release/experimentos/%.o: experimentos/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

build/release/tests/%.o: tests/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(COMMON_CFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

build/test/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(COMMON_CFLAGS) $(TEST_CFLAGS) -MMD -MP -c -o $@ $<

build/test/tests/%.o: tests/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(COMMON_CFLAGS) $(TEST_CFLAGS) -MMD -MP -c -o $@ $<

clean:
	rm -rf build partida_sim.out

-include $(DEPS)
