/**
 * @file mus.h
 * @brief Reglas del mus: valoración de manos, lances y gestión de partida.
 *
 * Salvo que se indique lo contrario, las funciones devuelven 0 si éxito y
 * 1 si error. Las funciones ganador* devuelven el índice (0-3) del jugador
 * que gana el lance; los empates los gana el más cercano a la mano.
 */
#ifndef MUS_H
#define MUS_H

#include <stddef.h>

#include "cartas.h"

#define TAMANO_MANO_MUS 4
#define NUMERO_JUGADORES_MUS 4

/** Clases de valor del mus: as y dos son pitos, tres y rey cerdos. */
typedef enum {
    PITO,
    MUS_CUATRO,
    MUS_CINCO,
    MUS_SEIS,
    MUS_SIETE,
    MUS_SOTA,
    MUS_CABALLO,
    CERDO
} NumeroMus;

/** Jugadas del lance de pares, de peor a mejor. */
enum { NO_PAR, PAR, MEDIAS, DUPLEX };

/** Estado de una partida por parejas: jugadores 0-2 contra 1-3. */
typedef struct {
    Baraja baraja;    /**< Mazo de robo. */
    Baraja descartes; /**< Pila de descartes; se recicla al agotar el mazo. */
    Mano manos[NUMERO_JUGADORES_MUS];
    int tantos[2]; /**< Tantos acumulados por cada pareja. */
    int mano;      /**< Jugador que es mano en la ronda actual. */
} PartidaMus;

/** Reserva una mano de TAMANO_MANO_MUS cartas. */
int crearManoMus(Mano *mano);

/** Clase NumeroMus a la que pertenece la carta. */
int valorMus(Carta carta);

/** Clave comparable de la mano para grande: mayor clave, mejor mano. */
int claveGrande(Mano mano);

/** Ganador del lance de grande. */
int ganadorGrande(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Clave comparable para chica (las cartas bajas puntúan más). */
int claveChica(Mano mano);

/** Ganador del lance de chica. */
int ganadorChica(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Clave comparable de pares: 0 sin pares; codifica jugada y alturas. */
int clavePar(Mano mano);

/** Ganador del lance de pares. */
int ganadorPar(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Puntos de la carta para el juego: pitos 1, figuras y treses 10. */
int valorPuntoMus(Carta carta);

/** Suma de puntos de la mano para el lance de juego. */
int sumaMano(Mano mano);

/** Ganador del juego (orden 31 > 32 > 40 > 37 > ... > 33); si nadie
 *  llega a 31 el lance se decide al punto. */
int ganadorJuego(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Ganador al punto: la suma de puntos más alta. */
int ganadorPunto(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Crea mazo, descartes y manos; tantos a 0 y mano al jugador 0. */
int iniciarPartidaMus(PartidaMus *partida);

/** Libera toda la memoria de la partida. */
int destruirPartidaMus(PartidaMus *partida);

/** Reconstruye el mazo con la pila de descartes barajada. */
int barajarDescartes(PartidaMus *partida);

/** Rellena una mano robando del mazo; recicla los descartes si se agota. */
int repartirMano(PartidaMus *partida, Mano *mano);

/** Reparte las cuatro manos de la partida. */
int repartirManos(PartidaMus *partida);

/** Sustituye las cartas marcadas en descartadas (1 = descartar) robando
 *  del mazo; las descartadas van a la pila de descartes. */
int manoSeDescarta(PartidaMus *partida, Mano *mano,
                   int descartadas[TAMANO_MANO_MUS]);

/** Suma tantos a la pareja del ganador del lance. Devuelve 1 o 2 si esa
 *  pareja alcanza los 40 tantos, 0 si la partida sigue. */
int puntuarRonda(PartidaMus *partida, int ganador, int tantos);

/** Baraja el mazo completo y reinicia el cursor de robo. */
int resetearMazo(PartidaMus *partida);

#endif
