/**
 * @file mus_sim.h
 * @brief Simulación de partidas y cálculo combinatorio de probabilidades.
 */
#ifndef MUS_SIM_H
#define MUS_SIM_H

#include "mus.h"

/** Restricciones sobre las manos rivales enumeradas. */
typedef enum { NADA, TIENE_JUEGO, TIENE_31, PAR_Y_JUEGO } Condicion;

/** Comprueba una restricción probabilística; devuelve 0, 1 o -1 si error. */
int manoCumpleCondicion(Mano mano, Condicion condicion);

/** Cartas disponibles de cada clase NumeroMus. */
typedef struct {
    int c[CERDO + 1];
} ConteoMus;

/** Decide si el jugador da mus (1), corta (0) o informa de error (-1). */
typedef int (*DecisionMus)(const Mano *mano, int jugador, int manoPartida,
                           const int tantos[2], void *contexto);

/** Marca con 1 las cartas que el jugador descarta; devuelve 0 si éxito. */
typedef int (*SeleccionDescartes)(const Mano *mano, int jugador,
                                  int descartadas[TAMANO_MANO_MUS],
                                  void *contexto);

/** Decide la siguiente acción de un jugador en la negociación de un lance. */
typedef AccionEnviteMus (*DecisionEnvite)(const Mano *mano, int jugador,
                                          int manoPartida,
                                          const int tantos[2], Ronda ronda,
                                          const EnviteMus *envite,
                                          void *contexto);

/** Decisiones inyectables de un jugador durante toda la mano. */
typedef struct {
    DecisionMus decidirMus;
    SeleccionDescartes elegirDescartes;
    DecisionEnvite decidirEnvite;
    void *contexto;
} EstrategiaMus;

/** Ejecuta mus y descartes repetidos hasta que un jugador corta. */
int jugarFaseMus(PartidaMus *partida,
                 const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]);

/** Negocia y registra el envite de un lance; devuelve 0, 1, 2 o -1. */
int jugarLanceEnvite(
    PartidaMus *partida, Ronda ronda,
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS],
    EnviteMus *resultado);

/** Juega una ronda con la estrategia pasiva: corta mus y pasa los envites.
 *  Reparte, tantea los lances aplicables y pasa la mano al siguiente jugador.
 *  @return 0 si la partida sigue, 1 o 2 si esa pareja llega a 40, -1 si
 *  error. */
int simularRondaMus(PartidaMus *partida);

/** Juega una ronda usando estrategias para mus, descartes y envites. */
int simularRondaMusConEstrategias(
    PartidaMus *partida,
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]);

/** Simula una partida completa a 40 tantos, logueando el progreso. */
int simularPartidaMus();

/** Simula una partida completa usando las cuatro estrategias indicadas. */
int simularPartidaMusConEstrategias(
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]);

/** Probabilidad exacta de que la pareja de los jugadores 0 y 2 gane el
 *  lance, fijadas sus manos y enumerando todos los repartos rivales.
 *  @param manos Manos fijas de los jugadores 0 y 2.
 *  @param mano Jugador que es mano (0-3).
 *  En PARES y JUEGO, un reparto sin ese lance cuenta como no ganado. PUNTO
 *  solo se puede ganar cuando ninguno de los cuatro jugadores tiene juego.
 *  @param ronda Lance a evaluar.
 *  @param condicionMano1 Restricción del jugador 1.
 *  @param condicionMano2 Restricción del jugador 3.
 *  @return Probabilidad en [0, 1], o -1.0 si las manos no son válidas. */
double probabilidadesVictoria2Fija(Mano manos[NUMERO_JUGADORES_MUS - 2],
                                   int mano, Ronda ronda,
                                   Condicion condicionMano1,
                                   Condicion condicionMano2);

#endif
