/**
 * @file mus_sim.h
 * @brief Simulación de partidas y cálculo combinatorio de probabilidades.
 */
#ifndef MUS_SIM_H
#define MUS_SIM_H

#include "mus.h"

/** Máximo de rondas de descartes consecutivas antes de abortar una fase. */
#ifndef MAXIMO_RONDAS_FASE_MUS
#define MAXIMO_RONDAS_FASE_MUS 1000
#endif

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

/** Decide la siguiente acción de un jugador en la negociación de un lance.
 *  ACCION_PASAR rechaza como ACCION_NO_QUERER un envite pendiente. */
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

/** Ejecuta mus y descartes hasta que alguien corta; devuelve 1 si alcanza el
 *  límite MAXIMO_RONDAS_FASE_MUS o una estrategia produce un error. */
int jugarFaseMus(PartidaMus *partida,
                 const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]);

/** Negocia y registra el envite de un lance; devuelve 0, 1, 2 o -1. */
int jugarLanceEnvite(
    PartidaMus *partida, Ronda ronda,
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS],
    EnviteMus *resultado);

/** Juega una ronda con la estrategia pasiva: corta mus y pasa los envites.
 *  Reconstruye y baraja el mazo, reparte, tantea los lances aplicables y pasa
 *  la mano al siguiente jugador.
 *  @return 0 si la partida sigue, 1 o 2 si ese equipo llega a 40, -1 si
 *  error. */
int simularRondaMus(PartidaMus *partida);

/** Reconstruye el mazo y juega una ronda usando las estrategias indicadas. */
int simularRondaMusConEstrategias(
    PartidaMus *partida,
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]);

/** Simula una partida completa a 40 tantos, logueando el progreso. */
int simularPartidaMus(void);

/** Simula una partida tradicional usando las cuatro estrategias indicadas. */
int simularPartidaMusConEstrategias(
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]);

/** Simula una partida pasiva completa para dos o cuatro jugadores. */
int simularPartidaMusConJugadores(int numeroJugadores);

/** Simula una partida completa para dos o cuatro jugadores usando una
 *  estrategia por cada jugador activo. */
int simularPartidaMusConEstrategiasYJugadores(
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS],
    int numeroJugadores);

/** Probabilidad exacta de que el jugador 0 gane a un único rival, fijada su
 *  mano y enumerando todas las manos físicas posibles del jugador 1.
 *  @param mano Mano física fija del jugador 0.
 *  @param manoPartida Jugador que es mano (0 o 1).
 *  @param ronda Lance a evaluar.
 *  @param condicionRival Restricción opcional de la mano rival.
 *  @return Probabilidad en [0, 1], o -1.0 si la entrada no es válida o el
 *  filtro deja vacío el espacio muestral. */
double probabilidadesVictoria1Fija(Mano mano, int manoPartida, Ronda ronda,
                                   Condicion condicionRival);

/** Probabilidad exacta de que la pareja de los jugadores 0 y 2 gane el
 *  lance, fijadas sus manos y enumerando todos los repartos rivales.
 *  @param manos Manos fijas de los jugadores 0 y 2.
 *  @param mano Jugador que es mano (0-3).
 *  En PARES y JUEGO, un reparto sin ese lance cuenta como no ganado. PUNTO
 *  solo se puede ganar cuando ninguno de los cuatro jugadores tiene juego.
 *  @param ronda Lance a evaluar.
 *  @param condicionMano1 Restricción del jugador 1.
 *  @param condicionMano2 Restricción del jugador 3.
 *  @return Probabilidad en [0, 1], o -1.0 si la entrada no es válida o los
 *  filtros dejan vacío el espacio muestral. */
double probabilidadesVictoria2Fija(Mano manos[NUMERO_JUGADORES_MUS - 2],
                                   int mano, Ronda ronda,
                                   Condicion condicionMano1,
                                   Condicion condicionMano2);

#endif
