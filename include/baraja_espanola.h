/**
 * @file baraja_espanola.h
 * @brief Baraja española: palos, números y creación de mazos de 40 y 48.
 */
#ifndef BARAJA_ESPANOLA_H
#define BARAJA_ESPANOLA_H

#include "cartas.h"

/** Palos de la baraja española. */
typedef enum { OROS, COPAS, ESPADAS, BASTOS } PaloEspanol;

/** Números de la baraja española (sota 10, caballo 11, rey 12). */
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

/** Crea la baraja de 40 cartas (sin ochos ni nueves). Devuelve 0 si éxito. */
int crearBarajaEspanola40(Baraja *baraja);

/** Crea la baraja completa de 48 cartas. Devuelve 0 si éxito. */
int crearBarajaEspanola48(Baraja *baraja);

#endif
