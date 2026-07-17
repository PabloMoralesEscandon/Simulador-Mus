#ifndef MUS_IO_H
#define MUS_IO_H

#include "mus.h"

int imprimirCarta(Carta carta);

int imprimirMano(Mano mano);

int imprimirManos(const PartidaMus *partida);

int imprimirNumeroRonda(int ronda);

int imprimirGanadorLance(const char *lance, int ganador);

int imprimirTantos(const PartidaMus *partida);

int imprimirGanadorPartida(const PartidaMus *partida);

#endif
