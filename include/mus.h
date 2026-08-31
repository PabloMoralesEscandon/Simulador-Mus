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

/** Modalidades de partida admitidas. NUMERO_JUGADORES_MUS sigue siendo la
 *  capacidad máxima y conserva la compatibilidad con la API de cuatro. */
typedef enum {
    MUS_DOS_JUGADORES = 2,
    MUS_CUATRO_JUGADORES = NUMERO_JUGADORES_MUS
} NumeroJugadoresMus;

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

/** Negociación activa de un lance entre los dos equipos. */
typedef struct {
    EstadoEnviteMus estado;
    int cantidad;
    int cantidadAnterior;
    int parejaApostadora;
} EnviteMus;

/** Acciones que puede devolver una estrategia durante un lance.
 *  ACCION_PASAR solo es válida mientras no haya un envite pendiente. */
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

/** Estado de una partida. Con dos jugadores, cada uno forma un equipo; con
 *  cuatro, juegan 0-2 contra 1-3. */
typedef struct {
    Baraja baraja;    /**< Mazo de robo. */
    Baraja descartes; /**< Pila de descartes; se recicla al agotar el mazo. */
    Mano manos[NUMERO_JUGADORES_MUS];
    int tantos[2]; /**< Tantos acumulados por cada equipo. */
    int numeroJugadores; /**< Jugadores activos: 2 o 4. */
    int mano;      /**< Jugador que es mano en la ronda actual. */
    EnviteRonda envites_actuales;
} PartidaMus;

/** Reinicia a cero los envites registrados para una ronda. */
int reiniciarEnvitesRonda(EnviteRonda *envites);

/** Inicializa una negociación de envite al paso. */
int iniciarEnviteMus(EnviteMus *envite);

/** Abre o sube un envite hasta una cantidad total mínima de dos tantos. */
int envidarMus(EnviteMus *envite, int pareja, int cantidadTotal);

/** Acepta el envite u órdago pendiente desde el equipo contrario. */
int quererEnviteMus(EnviteMus *envite, int pareja);

/** Rechaza el envite u órdago pendiente desde el equipo contrario. */
int noQuererEnviteMus(EnviteMus *envite, int pareja);

/** Abre un órdago o lo usa para responder al envite contrario. */
int ordagoMus(EnviteMus *envite, int pareja);

/** Registra un envite resuelto; devuelve 0, equipo ganador, o -1 si error. */
int registrarEnviteMus(PartidaMus *partida, Ronda ronda,
                       const EnviteMus *envite);

/** Resuelve un órdago aceptado y entrega el juego completo; -1 si error. */
int resolverOrdagoMus(PartidaMus *partida, Ronda ronda,
                      const EnviteMus *envite);

/** Aplica una acción del equipo indicado a una negociación de envite.
 *  Pasar ante un envite u órdago pendiente es un error: la estrategia debe
 *  responder con querer, no querer, subir el envite o lanzar un órdago. */
int aplicarAccionEnviteMus(EnviteMus *envite, int pareja,
                           AccionEnviteMus accion);

/** Reserva una mano de TAMANO_MANO_MUS cartas. */
int crearManoMus(Mano *mano);

/** Clase NumeroMus a la que pertenece la carta; -1 si el número no es válido. */
int valorMus(Carta carta);

/** Clave comparable de la mano para grande; -1 si contiene una carta inválida. */
int claveGrande(Mano mano);

/** Ganador del lance de grande. */
int ganadorGrande(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Ganador de grande entre dos o cuatro jugadores. */
int ganadorGrandeConJugadores(Mano manos[], int numeroJugadores, int mano);

/** Puntúa grande y su envite aceptado; 0, 1 o 2 si éxito, -1 si error. */
int puntuarGrande(PartidaMus *partida);

/** Clave para chica (las bajas puntúan más); -1 con una carta inválida. */
int claveChica(Mano mano);

/** Ganador del lance de chica. */
int ganadorChica(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Ganador de chica entre dos o cuatro jugadores. */
int ganadorChicaConJugadores(Mano manos[], int numeroJugadores, int mano);

/** Puntúa chica y su envite aceptado; 0, 1 o 2 si éxito, -1 si error. */
int puntuarChica(PartidaMus *partida);

/** Clave de pares: 0 sin pares, -1 si hay una carta inválida. */
int clavePar(Mano mano);

/** Tipo de pares: NO_PAR, PAR, MEDIAS o DUPLEX; -1 si la mano no es válida. */
int tipoPares(Mano mano);

/** Tantos intrínsecos de pares (0-3), o -1 si la mano no es válida. */
int tantosPares(Mano mano);

/** Ganador del lance de pares. */
int ganadorPar(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Ganador de pares entre dos o cuatro jugadores. */
int ganadorParConJugadores(Mano manos[], int numeroJugadores, int mano);

/** Indica si un equipo tiene pares. En la modalidad de dos jugadores, el
 *  equipo coincide con el jugador; -1 si los argumentos no son válidos. */
int equipoTienePares(Mano manos[], int numeroJugadores, int equipo);

/** Indica si algún jugador de la pareja 0 o 1 tiene pares; -1 si error. */
int parejaTienePares(Mano manos[NUMERO_JUGADORES_MUS], int pareja);

/** Puntúa los pares y su envite aceptado; 0, 1 o 2 si éxito, -1 si error. */
int puntuarPares(PartidaMus *partida);

/** Puntúa los pares de un equipo ganador y un envite adicional. */
int puntuarParesDePareja(PartidaMus *partida, int pareja, int tantosEnvite);

/** Puntos para juego: pitos 1, figuras y treses 10; -1 si no es válida. */
int valorPuntoMus(Carta carta);

/** Suma de puntos para juego, o -1 si la mano contiene una carta inválida. */
int sumaMano(Mano mano);

/** Indica si la mano tiene juego (0/1), o -1 si contiene una carta inválida. */
int tieneJuego(Mano mano);

/** Tantos intrínsecos de juego (0, 2 o 3), o -1 si la mano no es válida. */
int tantosJuego(Mano mano);

/** Indica si algún jugador de la pareja 0 o 1 tiene juego; -1 si error. */
int parejaTieneJuego(Mano manos[NUMERO_JUGADORES_MUS], int pareja);

/** Indica si un equipo tiene juego. En la modalidad de dos jugadores, el
 *  equipo coincide con el jugador; -1 si los argumentos no son válidos. */
int equipoTieneJuego(Mano manos[], int numeroJugadores, int equipo);

/** Puntúa juego o punto y su envite; 0, 1 o 2 si éxito, -1 si error. */
int puntuarJuegoOPunto(PartidaMus *partida);

/** Puntúa juego o punto para un equipo ganador y un envite adicional. */
int puntuarJuegoOPuntoDePareja(PartidaMus *partida, Ronda ronda, int pareja,
                               int tantosEnvite);

/** Ganador del juego (orden 31 > 32 > 40 > 37 > ... > 33); si nadie
 *  llega a 31 el lance se decide al punto. */
int ganadorJuego(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Ganador de juego entre dos o cuatro jugadores. */
int ganadorJuegoConJugadores(Mano manos[], int numeroJugadores, int mano);

/** Ganador al punto: la suma de puntos más alta. */
int ganadorPunto(Mano manos[NUMERO_JUGADORES_MUS], int mano);

/** Ganador al punto entre dos o cuatro jugadores. */
int ganadorPuntoConJugadores(Mano manos[], int numeroJugadores, int mano);

/** Crea mazo, descartes y manos; tantos a 0 y mano al jugador 0. */
int iniciarPartidaMus(PartidaMus *partida);

/** Inicializa una partida para exactamente dos o cuatro jugadores. */
int iniciarPartidaMusConJugadores(PartidaMus *partida, int numeroJugadores);

/** Libera toda la memoria de la partida. */
int destruirPartidaMus(PartidaMus *partida);

/** Sustituye el mazo agotado por descartes barajados; falla sin mutar si no hay. */
int barajarDescartes(PartidaMus *partida);

/** Rellena una mano robando del mazo; recicla los descartes si se agota. */
int repartirMano(PartidaMus *partida, Mano *mano);

/** Reparte las manos de los jugadores activos. */
int repartirManos(PartidaMus *partida);

/** Sustituye las cartas marcadas en descartadas (1 = descartar) robando
 *  del mazo; las descartadas van a la pila de descartes. */
int manoSeDescarta(PartidaMus *partida, Mano *mano,
                   int descartadas[TAMANO_MANO_MUS]);

/** Devuelve 1 si los cuatro jugadores dan mus, 0 si alguno corta, -1 si error. */
int todosDanMus(const int decisiones[NUMERO_JUGADORES_MUS]);

/** Como todosDanMus, para dos o cuatro jugadores. */
int todosDanMusConJugadores(const int decisiones[], int numeroJugadores);

/** Aplica los descartes de los jugadores activos empezando por la mano. */
int descartarManosMus(
    PartidaMus *partida,
    int descartadas[NUMERO_JUGADORES_MUS][TAMANO_MANO_MUS]);

/** Suma tantos al equipo del ganador y limita su marcador a 40.
 *  Devuelve 1 o 2 si hay ganador, 0 si la partida sigue y -1 si hay error. */
int puntuarRonda(PartidaMus *partida, int ganador, int tantos);

/** Reconstruye y baraja las 40 cartas entre manos; vacía los descartes. */
int resetearMazo(PartidaMus *partida);

#endif
