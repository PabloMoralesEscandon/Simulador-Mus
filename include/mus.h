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

int crearManoMus(Mano *mano);

int ganadorGrande(Mano manos[NUMERO_JUGADORES_MUS], int mano);

int ganadorChica(Mano manos[NUMERO_JUGADORES_MUS], int mano);

int ganadorPar(Mano manos[NUMERO_JUGADORES_MUS], int mano);

int ganadorPunto(Mano manos[NUMERO_JUGADORES_MUS], int mano);

#endif
