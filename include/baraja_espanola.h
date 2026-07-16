#ifndef BARAJA_ESPANOLA_H
#define BARAJA_ESPANOLA_H

#include "cartas.h"

typedef enum { OROS, COPAS, ESPADAS, BASTOS } PaloEspanol;

typedef enum {
    AS = 1,
    DOS = 2,
    TRES = 3,
    CUATRO = 4,
    CINCO = 5,
    SEIS = 6,
    SIETE = 7,
    OCHO = 8,
    NUEVE = 9,
    SOTA = 10,
    CABALLO = 11,
    REY = 12
} NumeroEspanol;

int crearBarajaEspanola40(Baraja *baraja);

int crearBarajaEspanola48(Baraja *baraja);

#endif
