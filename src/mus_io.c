#include <stdio.h>

#include "baraja_espanola.h"
#include "mus_io.h"

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

int imprimirCarta(Carta carta) {
    const char *numero = nombreNumero(carta.numero);
    const char *palo = nombrePalo(carta.palo);
    if (numero == NULL || palo == NULL)
        return 1;
    printf("%s de %s", numero, palo);
    return 0;
}

int imprimirMano(Mano mano) {
    if (mano.cartas == NULL)
        return 1;
    for (size_t i = 0; i < mano.tamano; i++) {
        if (i > 0)
            printf(", ");
        if (imprimirCarta(mano.cartas[i]))
            return 1;
    }
    return 0;
}

int imprimirManos(const PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    for (int jugador = 0; jugador < NUMERO_JUGADORES_MUS; jugador++) {
        printf("Jugador %d%s: ", jugador,
               jugador == partida->mano ? " (mano)" : "");
        if (imprimirMano(partida->manos[jugador]))
            return 1;
        printf("\n");
    }
    return 0;
}

int imprimirNumeroRonda(int ronda) {
    if (ronda < 1)
        return 1;
    printf("\n=== Ronda %d ===\n", ronda);
    return 0;
}

int imprimirGanadorLance(const char *lance, int ganador) {
    if (lance == NULL || ganador < 0 || ganador >= NUMERO_JUGADORES_MUS)
        return 1;
    printf("%s: gana el jugador %d (pareja %d)\n", lance, ganador,
           ganador % 2);
    return 0;
}

int imprimirTantos(const PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    printf("Tantos: pareja 0 (jugadores 0 y 2) %d - %d pareja 1 (jugadores 1 "
           "y 3)\n",
           partida->tantos[0], partida->tantos[1]);
    return 0;
}

int imprimirGanadorPartida(const PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    int pareja;
    if (partida->tantos[0] >= 40)
        pareja = 0;
    else if (partida->tantos[1] >= 40)
        pareja = 1;
    else
        return 1;
    printf("\nGana la pareja %d (jugadores %d y %d) por %d a %d\n", pareja,
           pareja, pareja + 2, partida->tantos[pareja],
           partida->tantos[1 - pareja]);
    return 0;
}
