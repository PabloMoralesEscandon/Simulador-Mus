/**
 * @file mus_log.h
 * @brief Logger de partidas: niveles de detalle y salida configurable.
 *
 * Las funciones log* devuelven 0 tanto si escriben como si el nivel
 * configurado las filtra, y 1 si los argumentos son inválidos.
 */
#ifndef MUS_LOG_H
#define MUS_LOG_H

#include <stdio.h>

#include "mus.h"

/** Niveles de detalle acumulativos: un mensaje se escribe si su nivel es
 *  menor o igual al configurado. LOG_SILENCIO (por defecto) apaga todo. */
typedef enum {
    LOG_SILENCIO,
    LOG_RESULTADO,
    LOG_RONDAS,
    LOG_LANCES
} NivelLog;

#if defined(__GNUC__)
#define FORMATO_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#define FORMATO_PRINTF(fmt, args)
#endif

/** Fija el nivel máximo de detalle que se escribe. */
void fijarNivelLog(NivelLog nivel);

/** Nivel configurado actualmente. */
NivelLog obtenerNivelLog(void);

/** Redirige el log a salida; NULL vuelve a la salida por defecto (stdout). */
void fijarSalidaLog(FILE *salida);

/** Escribe un mensaje libre estilo printf con el nivel dado. */
int logMus(NivelLog nivel, const char *formato, ...) FORMATO_PRINTF(2, 3);

/** Escribe una carta como "Rey de Oros" (sin salto de línea). */
int logCarta(NivelLog nivel, Carta carta);

/** Escribe las cartas de una mano separadas por comas. */
int logMano(NivelLog nivel, Mano mano);

/** Escribe las cuatro manos de la partida señalando quién es mano. */
int logManos(NivelLog nivel, const PartidaMus *partida);

/** Escribe la cabecera de una ronda (numeradas desde 1). */
int logNumeroRonda(NivelLog nivel, int ronda);

/** Escribe el jugador y la pareja que ganan un lance. */
int logGanadorLance(NivelLog nivel, const char *lance, int ganador);

/** Escribe el marcador de tantos de las dos parejas. */
int logTantos(NivelLog nivel, const PartidaMus *partida);

/** Escribe la pareja ganadora; falla si nadie ha llegado a 40 tantos. */
int logGanadorPartida(NivelLog nivel, const PartidaMus *partida);

#endif
