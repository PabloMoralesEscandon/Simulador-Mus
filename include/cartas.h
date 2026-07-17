/**
 * @file cartas.h
 * @brief Tipos y operaciones genéricas sobre cartas, manos y barajas.
 *
 * Salvo que se indique lo contrario, las funciones devuelven 0 en caso de
 * éxito y 1 en caso de error (punteros NULL, memoria agotada, etc.).
 */
#ifndef CARTAS_H
#define CARTAS_H

#include <stddef.h>

/** Carta genérica; el significado de numero y palo lo fija cada baraja. */
typedef struct {
    int numero;
    int palo;
} Carta;

/** Cartas en juego de un jugador. */
typedef struct {
    Carta *cartas;
    size_t tamano;
} Mano;

/** Mazo cuyo cursor siguiente_carta avanza según se roban cartas. */
typedef struct {
    Carta *cartas;
    size_t tamano;
    size_t siguiente_carta;
} Baraja;

/** Reserva memoria para una baraja de tamano cartas y reinicia su cursor. */
int crearBaraja(Baraja *baraja, size_t tamano);

/** Libera la memoria de la baraja. */
int destruirBaraja(Baraja *baraja);

/** Baraja las cartas (Fisher-Yates) sin tocar el cursor de robo. */
int barajar(Baraja *baraja);

/** Copia en out la siguiente carta y avanza el cursor; 1 si no quedan. */
int sacarCarta(Baraja *baraja, Carta *out);

/** Reserva memoria para una mano de tamano cartas. */
int crearMano(Mano *mano, size_t tamano);

/** Libera la memoria de la mano. */
int destruirMano(Mano *mano);

/** Ordena las cartas de la mano por numero ascendente. */
int ordenarMano(Mano *mano);

#endif
