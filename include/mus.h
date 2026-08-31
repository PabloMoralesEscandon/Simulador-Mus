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

/** Lances de una mano de mus. */
typedef enum { GRANDE, CHICA, PARES, JUEGO, PUNTO } Ronda;

/** Estado de una negociación de envite. */
typedef enum {
    ENVITE_AL_PASO,
    ENVITE_PENDIENTE,
    ENVITE_ACEPTADO,
    ENVITE_RECHAZADO,
    ORDAGO_PENDIENTE,
    ORDAGO_ACEPTADO,
    ORDAGO_RECHAZADO
} EstadoEnviteMus;

/** Negociación activa de un lance entre las dos parejas. */
typedef struct {
    EstadoEnviteMus estado;
    int cantidad;
    int cantidadAnterior;
    int parejaApostadora;
} EnviteMus;

/** Acciones que puede devolver una estrategia durante un lance. */
typedef enum {
    ACCION_PASAR,
    ACCION_ENVIDAR,
    ACCION_QUERER,
    ACCION_NO_QUERER,
    ACCION_ORDAGO
} TipoAccionEnviteMus;

/** Acción de apuesta; cantidadTotal solo se usa con ACCION_ENVIDAR. */
typedef struct {
    TipoAccionEnviteMus tipo;
    int cantidadTotal;
} AccionEnviteMus;

/** Importes aceptados que se tantean al final de la mano. */
typedef struct {
    int grande;
    int chica;
    int pares;
    int juego;
    int punto;
} EnviteRonda;

/** Estado de una partida por parejas: jugadores 0-2 contra 1-3. */
typedef struct {
    Baraja baraja;    /**< Mazo de robo. */
    Baraja descartes; /**< Pila de descartes; se recicla al agotar el mazo. */
    Mano manos[NUMERO_JUGADORES_MUS];
    int tantos[2]; /**< Tantos acumulados por cada pareja. */
    int mano;      /**< Jugador que es mano en la ronda actual. */
    EnviteRonda envites_actuales;
} PartidaMus;

/** Reinicia a cero los envites registrados para una ronda. */
int reiniciarEnvitesRonda(EnviteRonda *envites);

/** Inicializa una negociación de envite al paso. */
int iniciarEnviteMus(EnviteMus *envite);

/** Abre o sube un envite hasta una cantidad total mínima de dos tantos. */
int envidarMus(EnviteMus *envite, int pareja, int cantidadTotal);

/** Acepta el envite u órdago pendiente desde la pareja contraria. */
int quererEnviteMus(EnviteMus *envite, int pareja);

/** Rechaza el envite u órdago pendiente desde la pareja contraria. */
int noQuererEnviteMus(EnviteMus *envite, int pareja);

/** Abre un órdago o lo usa para responder al envite contrario. */
int ordagoMus(EnviteMus *envite, int pareja);

/** Registra un envite resuelto; devuelve 0, pareja ganadora, o -1 si error. */
int registrarEnviteMus(PartidaMus *partida, Ronda ronda,
                       const EnviteMus *envite);

/** Resuelve un órdago aceptado y entrega el juego completo; -1 si error. */
int resolverOrdagoMus(PartidaMus *partida, Ronda ronda,
                      const EnviteMus *envite);

/** Aplica una acción de la pareja indicada a una negociación de envite. */
int aplicarAccionEnviteMus(EnviteMus *envite, int pareja,
                           AccionEnviteMus accion);

/** Reserva una mano de TAMANO_MANO_MUS cartas. */
int crearManoMus(Mano *mano);

/** Clase NumeroMus a la que pertenece la carta. */
int valorMus(Carta carta);

/** Clave comparable de la mano para grande: mayor clave, mejor mano. */
int claveGrande(Mano mano);

/** Ganador del lance de grande. */
int ganadorGrande(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Puntúa grande y su envite aceptado; 0, 1 o 2 si éxito, -1 si error. */
int puntuarGrande(PartidaMus *partida);

/** Clave comparable para chica (las cartas bajas puntúan más). */
int claveChica(Mano mano);

/** Ganador del lance de chica. */
int ganadorChica(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Puntúa chica y su envite aceptado; 0, 1 o 2 si éxito, -1 si error. */
int puntuarChica(PartidaMus *partida);

/** Clave comparable de pares: 0 sin pares; codifica jugada y alturas. */
int clavePar(Mano mano);

/** Tipo de pares de la mano: NO_PAR, PAR, MEDIAS o DUPLEX. */
int tipoPares(Mano mano);

/** Tantos por el valor intrínseco de los pares: 0, 1, 2 o 3. */
int tantosPares(Mano mano);

/** Ganador del lance de pares. */
int ganadorPar(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Indica si algún jugador de la pareja 0 o 1 tiene pares; -1 si error. */
int parejaTienePares(Mano manos[NUMERO_JUGADORES_MUS], int pareja);

/** Puntúa los pares y su envite aceptado; 0, 1 o 2 si éxito, -1 si error. */
int puntuarPares(PartidaMus *partida);

/** Puntúa los pares de una pareja ganadora y un envite adicional. */
int puntuarParesDePareja(PartidaMus *partida, int pareja, int tantosEnvite);

/** Puntos de la carta para el juego: pitos 1, figuras y treses 10. */
int valorPuntoMus(Carta carta);

/** Suma de puntos de la mano para el lance de juego. */
int sumaMano(Mano mano);

/** Indica si la mano suma 31 o más y, por tanto, tiene juego. */
int tieneJuego(Mano mano);

/** Tantos por el valor intrínseco del juego: 3 por 31, 2 por otro juego. */
int tantosJuego(Mano mano);

/** Indica si algún jugador de la pareja 0 o 1 tiene juego; -1 si error. */
int parejaTieneJuego(Mano manos[NUMERO_JUGADORES_MUS], int pareja);

/** Puntúa juego o punto y su envite; 0, 1 o 2 si éxito, -1 si error. */
int puntuarJuegoOPunto(PartidaMus *partida);

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

/** Devuelve 1 si los cuatro jugadores dan mus, 0 si alguno corta, -1 si error. */
int todosDanMus(const int decisiones[NUMERO_JUGADORES_MUS]);

/** Aplica los descartes de los cuatro jugadores empezando por la mano. */
int descartarManosMus(
    PartidaMus *partida,
    int descartadas[NUMERO_JUGADORES_MUS][TAMANO_MANO_MUS]);

/** Suma tantos a la pareja del ganador del lance. Devuelve 1 o 2 si esa
 *  pareja alcanza los 40 tantos, 0 si la partida sigue. */
int puntuarRonda(PartidaMus *partida, int ganador, int tantos);

/** Baraja el mazo completo y reinicia el cursor de robo. */
int resetearMazo(PartidaMus *partida);

#endif
