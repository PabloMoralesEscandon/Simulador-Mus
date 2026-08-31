/**
 * @file partida_sim.c
 * @brief Simula una partida de mus completa; admite el nivel de log
 *        (0 silencio - 3 lances) y 2 o 4 jugadores como argumentos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "mus_log.h"
#include "mus_sim.h"

static void imprimirUso(const char *programa) {
    fprintf(stderr,
            "Uso: %s [nivel] [jugadores]\n"
            "  nivel: 0 silencio, 1 resultado, 2 rondas, 3 lances "
            "(por defecto)\n"
            "  jugadores: 2 o 4 (4 por defecto)\n",
            programa);
}

int main(int argc, char *argv[]) {
    NivelLog nivel = LOG_LANCES;
    int numeroJugadores = MUS_CUATRO_JUGADORES;
    if (argc > 3) {
        imprimirUso(argv[0]);
        return 1;
    }
    if (argc > 1) {
        char *fin = NULL;
        long valor = strtol(argv[1], &fin, 10);
        if (argv[1][0] == '\0' || *fin != '\0' || valor < LOG_SILENCIO ||
            valor > LOG_LANCES) {
            imprimirUso(argv[0]);
            return 1;
        }
        nivel = (NivelLog)valor;
    }
    if (argc > 2) {
        char *fin = NULL;
        long valor = strtol(argv[2], &fin, 10);
        if (argv[2][0] == '\0' || *fin != '\0' ||
            (valor != MUS_DOS_JUGADORES &&
             valor != MUS_CUATRO_JUGADORES)) {
            imprimirUso(argv[0]);
            return 1;
        }
        numeroJugadores = (int)valor;
    }
    // El PID evita repetir semilla al lanzar varias ejecuciones dentro
    // del mismo segundo
    srand((unsigned)time(NULL) ^ ((unsigned)getpid() << 16));
    fijarNivelLog(nivel);
    return simularPartidaMusConJugadores(numeroJugadores);
}
