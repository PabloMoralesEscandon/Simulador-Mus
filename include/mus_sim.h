#ifndef MUS_SIM_H
#define MUS_SIM_H

#include "mus.h"

typedef enum { GRANDE, CHICA, PARES, JUEGO } Ronda;
typedef enum { NADA, TIENE_JUEGO, TIENE_31, PAR_Y_JUEGO } Condicion;

typedef struct {
    int c[CERDO + 1];
} ConteoMus;

int simularRondaMus(PartidaMus *partida);

int simularPartidaMus();

double probabilidadesVictoria2Fija(Mano manos[NUMERO_JUGADORES_MUS - 2],
                                   int mano, Ronda ronda,
                                   Condicion condicionMano1,
                                   Condicion condicionMano2);

#endif
