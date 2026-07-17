/**
 * @file mus_sim.h
 * @brief Simulación de partidas y cálculo combinatorio de probabilidades.
 */
#ifndef MUS_SIM_H
#define MUS_SIM_H

#include "mus.h"

/** Lances del mus. */
typedef enum { GRANDE, CHICA, PARES, JUEGO } Ronda;

/** Restricciones sobre las manos libres al simular (aún sin uso). */
typedef enum { NADA, TIENE_JUEGO, TIENE_31, PAR_Y_JUEGO } Condicion;

/** Cartas disponibles de cada clase NumeroMus. */
typedef struct {
    int c[CERDO + 1];
} ConteoMus;

/** Juega una ronda: reparte y puntúa los cuatro lances (1 tanto cada uno)
 *  y pasa la mano al siguiente jugador.
 *  @return 0 si la partida sigue, 1 o 2 si esa pareja llega a 40, -1 si
 *  error. */
int simularRondaMus(PartidaMus *partida);

/** Simula una partida completa a 40 tantos, logueando el progreso. */
int simularPartidaMus();

/** Probabilidad exacta de que la pareja de los jugadores 0 y 2 gane el
 *  lance, fijadas sus manos y enumerando todos los repartos rivales.
 *  @param manos Manos fijas de los jugadores 0 y 2.
 *  @param mano Jugador que es mano (0-3).
 *  @param ronda Lance a evaluar (de momento solo GRANDE).
 *  @param condicionMano1 Restricción del jugador 1 (aún sin uso).
 *  @param condicionMano2 Restricción del jugador 3 (aún sin uso).
 *  @return Probabilidad en [0, 1], o -1.0 si las manos no son válidas. */
double probabilidadesVictoria2Fija(Mano manos[NUMERO_JUGADORES_MUS - 2],
                                   int mano, Ronda ronda,
                                   Condicion condicionMano1,
                                   Condicion condicionMano2);

#endif
