#include <stdarg.h>
#include <stdio.h>

#include "baraja_espanola.h"
#include "mus_log.h"

static NivelLog nivel_log = LOG_SILENCIO;
static FILE *salida_log = NULL;

void fijarNivelLog(NivelLog nivel) { nivel_log = nivel; }

NivelLog obtenerNivelLog(void) { return nivel_log; }

void fijarSalidaLog(FILE *salida) { salida_log = salida; }

static FILE *salidaLog(void) {
    return salida_log != NULL ? salida_log : stdout;
}

static int nivelInvalido(NivelLog nivel) {
    return nivel <= LOG_SILENCIO || nivel > LOG_LANCES;
}

static int filtrado(NivelLog nivel) { return nivel > nivel_log; }

static const char *nombreNumero(int numero) {
    switch (numero) {
    case AS:
        return "As";
    case DOS:
        return "Dos";
    case TRES:
        return "Tres";
    case CUATRO:
        return "Cuatro";
    case CINCO:
        return "Cinco";
    case SEIS:
        return "Seis";
    case SIETE:
        return "Siete";
    case OCHO:
        return "Ocho";
    case NUEVE:
        return "Nueve";
    case SOTA:
        return "Sota";
    case CABALLO:
        return "Caballo";
    case REY:
        return "Rey";
    default:
        return NULL;
    }
}

static const char *nombrePalo(int palo) {
    switch (palo) {
    case OROS:
        return "Oros";
    case COPAS:
        return "Copas";
    case ESPADAS:
        return "Espadas";
    case BASTOS:
        return "Bastos";
    default:
        return NULL;
    }
}

int logMus(NivelLog nivel, const char *formato, ...) {
    if (nivelInvalido(nivel) || formato == NULL)
        return 1;
    if (filtrado(nivel))
        return 0;
    va_list argumentos;
    va_start(argumentos, formato);
    vfprintf(salidaLog(), formato, argumentos);
    va_end(argumentos);
    return 0;
}

int logCarta(NivelLog nivel, Carta carta) {
    const char *numero = nombreNumero(carta.numero);
    const char *palo = nombrePalo(carta.palo);
    if (nivelInvalido(nivel) || numero == NULL || palo == NULL)
        return 1;
    if (filtrado(nivel))
        return 0;
    fprintf(salidaLog(), "%s de %s", numero, palo);
    return 0;
}

int logMano(NivelLog nivel, Mano mano) {
    if (nivelInvalido(nivel) || mano.cartas == NULL)
        return 1;
    for (size_t i = 0; i < mano.tamano; i++) {
        if (i > 0 && logMus(nivel, ", "))
            return 1;
        if (logCarta(nivel, mano.cartas[i]))
            return 1;
    }
    return 0;
}

int logManos(NivelLog nivel, const PartidaMus *partida) {
    if (nivelInvalido(nivel) || partida == NULL)
        return 1;
    for (int jugador = 0; jugador < NUMERO_JUGADORES_MUS; jugador++) {
        if (logMus(nivel, "Jugador %d%s: ", jugador,
                   jugador == partida->mano ? " (mano)" : ""))
            return 1;
        if (logMano(nivel, partida->manos[jugador]))
            return 1;
        if (logMus(nivel, "\n"))
            return 1;
    }
    return 0;
}

int logNumeroRonda(NivelLog nivel, int ronda) {
    if (ronda < 1)
        return 1;
    return logMus(nivel, "\n=== Ronda %d ===\n", ronda);
}

int logGanadorLance(NivelLog nivel, const char *lance, int ganador) {
    if (lance == NULL || ganador < 0 || ganador >= NUMERO_JUGADORES_MUS)
        return 1;
    return logMus(nivel, "%s: gana el jugador %d (pareja %d)\n", lance,
                  ganador, ganador % 2);
}

int logTantos(NivelLog nivel, const PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    return logMus(nivel,
                  "Tantos: pareja 0 (jugadores 0 y 2) %d - %d pareja 1 "
                  "(jugadores 1 y 3)\n",
                  partida->tantos[0], partida->tantos[1]);
}

int logGanadorPartida(NivelLog nivel, const PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    int pareja;
    if (partida->tantos[0] >= 40)
        pareja = 0;
    else if (partida->tantos[1] >= 40)
        pareja = 1;
    else
        return 1;
    return logMus(nivel, "\nGana la pareja %d (jugadores %d y %d) por %d a %d\n",
                  pareja, pareja, pareja + 2, partida->tantos[pareja],
                  partida->tantos[1 - pareja]);
}
