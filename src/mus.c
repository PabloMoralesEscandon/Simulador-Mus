#include <stdlib.h>

#include "baraja_espanola.h"
#include "mus.h"

static const int PESO_GRANDE[CERDO + 1] = {1,     10,     100,     1000,
                                           10000, 100000, 1000000, 10000000};
static const int PESO_CHICA[CERDO + 1] = {10000000, 1000000, 100000, 10000,
                                          1000,     100,     10,     1};
static const int ORDEN_PUNTO[8] = {33, 34, 35, 36, 37, 40, 32, 31};

int crearManoMus(Mano *mano) { return crearMano(mano, TAMANO_MANO_MUS); }

static int valorMus(Carta carta) {
    switch (carta.numero) {
    case AS:
    case DOS:
        return PITO;

    case TRES:
    case REY:
        return CERDO;

    case CUATRO:
        return MUS_CUATRO;

    case CINCO:
        return MUS_CINCO;

    case SEIS:
        return MUS_SEIS;

    case SIETE:
        return MUS_SIETE;

    case SOTA:
        return MUS_SOTA;

    case CABALLO:
        return MUS_CABALLO;

    default:
        return 0; // No se debería llegar aquí!!
    }
}

static int claveGrande(Mano mano) {
    int clave = 0;
    for (size_t i = 0; i < mano.tamano; i++) {
        clave += PESO_GRANDE[valorMus(mano.cartas[i])];
    }
    return clave;
}

int ganadorGrande(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    int max = claveGrande(manos[mano]);
    int ganador = mano;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++) {
        int j = (mano + i) % NUMERO_JUGADORES_MUS;
        int valor = claveGrande(manos[j]);
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

static int claveChica(Mano mano) {
    int clave = 0;
    for (size_t i = 0; i < mano.tamano; i++) {
        clave += PESO_CHICA[valorMus(mano.cartas[i])];
    }
    return clave;
}

int ganadorChica(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    int max = claveChica(manos[mano]);
    int ganador = mano;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++) {
        int j = (mano + i) % NUMERO_JUGADORES_MUS;
        int valor = claveChica(manos[j]);
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

static int clavePar(Mano mano) {
    int c[CERDO + 1] = {0};
    int tipo = NO_PAR;
    int alto = 0, bajo = 0;
    for (size_t i = 0; i < mano.tamano; i++)
        c[valorMus(mano.cartas[i])]++;
    for (int i = 0; i < CERDO + 1; i++) {
        if (c[i] == 4) {
            tipo = DUPLEX;
            alto = i + 1;
            bajo = alto;
            break;
        } else if (c[i] == 3) {
            tipo = MEDIAS;
            alto = i + 1;
            break;
        } else if (c[i] == 2) {
            if (tipo == PAR) {
                tipo = DUPLEX;
                if (alto < (i + 1)) {
                    bajo = alto;
                    alto = i + 1;
                } else
                    bajo = i + 1;
                break;
            } else {
                tipo = PAR;
                alto = i + 1;
            }
        }
    }
    switch (tipo) {
    case PAR:
        return alto;
    case MEDIAS:
        return alto * 10;
    case DUPLEX:
        return alto * 1000 + bajo * 100;
    default:
        return 0;
    }
}

int ganadorPar(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    int max = clavePar(manos[mano]);
    int ganador = mano;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++) {
        int j = (mano + i) % NUMERO_JUGADORES_MUS;
        int valor = clavePar(manos[j]);
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

static int valorPuntoMus(Carta carta) {
    int valor = valorMus(carta);

    switch (valor) {
    case PITO:
        return 1;
    case MUS_CUATRO:
        return 4;
    case MUS_CINCO:
        return 5;
    case MUS_SEIS:
        return 6;
    case MUS_SIETE:
        return 7;
    case MUS_SOTA:
    case MUS_CABALLO:
    case CERDO:
        return 10;
    default:
        return -1;
    }
}

static int sumaMano(Mano mano) {
    int cuenta = 0;

    // Loop through each card in the hand
    for (size_t i = 0; i < mano.tamano; i++) {
        cuenta += valorPuntoMus(mano.cartas[i]);
    }

    return cuenta; // Return the calculated sum
}

static int clavePunto(Mano mano) {
    int valor = sumaMano(mano);
    for (int i = 0; i < 8; i++)
        if (valor == ORDEN_PUNTO[i])
            return i;
    return -1;
}

int ganadorPunto(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    int max = clavePunto(manos[mano]);
    int ganador = mano;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++) {
        int j = (mano + i) % NUMERO_JUGADORES_MUS;
        int valor = clavePunto(manos[j]);
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}
