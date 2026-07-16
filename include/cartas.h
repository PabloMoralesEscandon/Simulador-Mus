#ifndef CARTAS_H
#define CARTAS_H

#include <stddef.h>

typedef struct {
    int numero;
    int palo;
} Carta;

typedef struct {
    Carta *cartas;
    size_t tamano;
} Mano;

typedef struct {
    Carta *cartas;
    size_t tamano;
    size_t siguiente_carta;
} Baraja;

int crearBaraja(Baraja *baraja, size_t tamano);

int destruirBaraja(Baraja *baraja);

int barajar(Baraja *baraja);

int sacarCarta(Baraja *baraja, Carta *out);

int crearMano(Mano *mano, size_t tamano);

int destruirMano(Mano *mano);

int ordenarMano(Mano *mano);

#endif
