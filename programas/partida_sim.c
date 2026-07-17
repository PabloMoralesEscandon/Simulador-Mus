#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "mus_log.h"
#include "mus_sim.h"

int main(int argc, char *argv[]) {
    NivelLog nivel = LOG_LANCES;
    if (argc > 1) {
        char *fin = NULL;
        long valor = strtol(argv[1], &fin, 10);
        if (argv[1][0] == '\0' || *fin != '\0' || valor < LOG_SILENCIO ||
            valor > LOG_LANCES) {
            fprintf(stderr,
                    "Uso: %s [nivel]\n"
                    "  0  silencio\n"
                    "  1  resultado de la partida\n"
                    "  2  + rondas y tantos\n"
                    "  3  + manos y lances (por defecto)\n",
                    argv[0]);
            return 1;
        }
        nivel = (NivelLog)valor;
    }
    srand((unsigned)time(NULL) ^ ((unsigned)getpid() << 16));
    fijarNivelLog(nivel);
    return simularPartidaMus();
}
