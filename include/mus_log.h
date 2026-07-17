#ifndef MUS_LOG_H
#define MUS_LOG_H

#include <stdio.h>

#include "mus.h"

// Cada mensaje lleva un nivel; solo se escribe si el nivel configurado
// lo alcanza. LOG_SILENCIO (por defecto) no escribe nada.
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

void fijarNivelLog(NivelLog nivel);

NivelLog obtenerNivelLog(void);

// NULL vuelve a la salida por defecto (stdout)
void fijarSalidaLog(FILE *salida);

int logMus(NivelLog nivel, const char *formato, ...) FORMATO_PRINTF(2, 3);

int logCarta(NivelLog nivel, Carta carta);

int logMano(NivelLog nivel, Mano mano);

int logManos(NivelLog nivel, const PartidaMus *partida);

int logNumeroRonda(NivelLog nivel, int ronda);

int logGanadorLance(NivelLog nivel, const char *lance, int ganador);

int logTantos(NivelLog nivel, const PartidaMus *partida);

int logGanadorPartida(NivelLog nivel, const PartidaMus *partida);

#endif
