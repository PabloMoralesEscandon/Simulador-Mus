#ifndef MUS_H
#define MUS_H

#include <stddef.h>

#include "cartas.h"

#define TAMANO_MANO_MUS 4
#define NUMERO_JUGADORES_MUS 4

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

enum { NO_PAR, PAR, MEDIAS, DUPLEX };

typedef struct {
    Baraja baraja;
    Baraja descartes;
    Mano manos[NUMERO_JUGADORES_MUS];
    int tantos[2];
    int mano;
} PartidaMus;

int crearManoMus(Mano *mano);

int valorMus(Carta carta);

int claveGrande(Mano mano);

int ganadorGrande(Mano manos[NUMERO_JUGADORES_MUS], int mano);

int claveChica(Mano mano);

int ganadorChica(Mano manos[NUMERO_JUGADORES_MUS], int mano);

int clavePar(Mano mano);

int ganadorPar(Mano manos[NUMERO_JUGADORES_MUS], int mano);

int valorPuntoMus(Carta carta);

int sumaMano(Mano mano);

int ganadorJuego(Mano manos[NUMERO_JUGADORES_MUS], int mano);

int iniciarPartidaMus(PartidaMus *partida);

int destruirPartidaMus(PartidaMus *partida);

int barajarDescartes(PartidaMus *partida);

int repartirMano(PartidaMus *partida, Mano *mano);

int repartirManos(PartidaMus *partida);

int manoSeDescarta(PartidaMus *partida, Mano *mano,
                   int descartadas[TAMANO_MANO_MUS]);

int puntuarRonda(PartidaMus *partida, int ganador, int tantos);

int resetearMazo(PartidaMus *partida);

#endif
