#include <stdlib.h>

#include "baraja_espanola.h"
#include "mus.h"

// Potencias de 10 por clase: al sumar los pesos de las 4 cartas, cada
// dígito de la clave cuenta cuántas cartas hay de esa clase, así que
// comparar claves equivale a comparar las manos carta a carta.
// PESO_CHICA invierte los pesos porque en chica mandan las cartas bajas.
static const int PESO_GRANDE[CERDO + 1] = {1,     10,     100,     1000,
                                           10000, 100000, 1000000, 10000000};
static const int PESO_CHICA[CERDO + 1] = {10000000, 1000000, 100000, 10000,
                                          1000,     100,     10,     1};
// Valores de juego ordenados de peor a mejor: 33 < 34 < ... < 40 < 32 < 31
static const int ORDEN_PUNTO[8] = {33, 34, 35, 36, 37, 40, 32, 31};

int crearManoMus(Mano *mano) { return crearMano(mano, TAMANO_MANO_MUS); }

int valorMus(Carta carta) {
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

int claveGrande(Mano mano) {
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

int claveChica(Mano mano) {
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

int clavePar(Mano mano) {
    int c[CERDO + 1] = {0};
    int tipo = NO_PAR;
    int alto = 0, bajo = 0;
    // Cuenta cuántas cartas hay de cada clase
    for (size_t i = 0; i < mano.tamano; i++)
        c[valorMus(mano.cartas[i])]++;
    // Busca repeticiones de menor a mayor clase; alto/bajo guardan i + 1
    // para que un par de pitos (i == 0) no se confunda con no tener pares
    for (int i = 0; i < CERDO + 1; i++) {
        if (c[i] == 4) {
            // Cuatro iguales: dúplex de la misma clase
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
                // Segundo par: dúplex con el par más alto por delante
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
    // La codificación separa los tipos por magnitud: cualquier dúplex
    // (>= 100) gana a cualquier medias (>= 10) y estas a cualquier par
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

int valorPuntoMus(Carta carta) {
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

int sumaMano(Mano mano) {
    int cuenta = 0;
    for (size_t i = 0; i < mano.tamano; i++) {
        cuenta += valorPuntoMus(mano.cartas[i]);
    }
    return cuenta;
}

/** Posición del juego en ORDEN_PUNTO (mayor es mejor); -1 sin juego. */
static int claveJuego(Mano mano) {
    int valor = sumaMano(mano);
    for (int i = 0; i < 8; i++)
        if (valor == ORDEN_PUNTO[i])
            return i;
    return -1;
}

int ganadorJuego(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    int max = claveJuego(manos[mano]);
    int ganador = mano;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++) {
        int j = (mano + i) % NUMERO_JUGADORES_MUS;
        int valor = claveJuego(manos[j]);
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    if (max == -1)
        return ganadorPunto(manos, mano);
    return ganador;
}

int ganadorPunto(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    int max = sumaMano(manos[mano]);
    int ganador = mano;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++) {
        int j = (mano + i) % NUMERO_JUGADORES_MUS;
        int valor = sumaMano(manos[j]);
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

int iniciarPartidaMus(PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    Baraja baraja = {0};
    if (crearBarajaEspanola40(&baraja))
        return 1;
    partida->baraja = baraja;
    Baraja descartes = {0};
    if (crearBarajaEspanola40(&descartes))
        return 1;
    partida->descartes = descartes;
    Mano mano1 = {0};
    if (crearMano(&mano1, TAMANO_MANO_MUS))
        return 1;
    partida->manos[0] = mano1;
    Mano mano2 = {0};
    if (crearMano(&mano2, TAMANO_MANO_MUS))
        return 1;
    partida->manos[1] = mano2;
    Mano mano3 = {0};
    if (crearMano(&mano3, TAMANO_MANO_MUS))
        return 1;
    partida->manos[2] = mano3;
    Mano mano4 = {0};
    if (crearMano(&mano4, TAMANO_MANO_MUS))
        return 1;
    partida->manos[3] = mano4;
    partida->tantos[0] = 0;
    partida->tantos[1] = 0;
    partida->mano = 0;
    return 0;
}

int destruirPartidaMus(PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    if (destruirBaraja(&partida->baraja))
        return 1;
    if (destruirBaraja(&partida->descartes))
        return 1;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++)
        if (destruirMano(&partida->manos[i]))
            return 1;
    partida->tantos[0] = 0;
    partida->tantos[1] = 0;
    partida->mano = 0;
    return 0;
}

int barajarDescartes(PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    // El mazo pasa a contener solo las cartas descartadas hasta ahora:
    // se copian al principio y tamano se encoge a ese número
    for (size_t i = 0; i < partida->descartes.siguiente_carta; i++)
        partida->baraja.cartas[i] = partida->descartes.cartas[i];
    partida->baraja.tamano = partida->descartes.siguiente_carta;
    if (barajar(&(partida->baraja)))
        return 1;
    partida->descartes.siguiente_carta = 0;
    partida->baraja.siguiente_carta = 0;
    return 0;
}

int repartirMano(PartidaMus *partida, Mano *mano) {
    if (partida == NULL)
        return 1;
    if (mano == NULL)
        return 1;
    for (size_t i = 0; i < mano->tamano; i++) {
        mano->cartas[i].numero =
            partida->baraja.cartas[partida->baraja.siguiente_carta].numero;
        mano->cartas[i].palo =
            partida->baraja.cartas[partida->baraja.siguiente_carta].palo;
        partida->baraja.siguiente_carta += 1;
        if (partida->baraja.siguiente_carta == partida->baraja.tamano)
            if (barajarDescartes(partida))
                return 1;
    }
    return 0;
}

int repartirManos(PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    for (size_t i = 0; i < NUMERO_JUGADORES_MUS; i++)
        if (repartirMano(partida, &(partida->manos[i])))
            return 1;
    return 0;
}

int manoSeDescarta(PartidaMus *partida, Mano *mano,
                   int descartadas[TAMANO_MANO_MUS]) {
    if (partida == NULL)
        return 1;
    if (mano == NULL)
        return 1;
    for (size_t i = 0; i < TAMANO_MANO_MUS; i++) {
        if (descartadas[i]) {
            // La carta descartada va a la pila y se roba una nueva;
            // si el mazo se agota, se recicla la pila de descartes
            partida->descartes.cartas[partida->descartes.siguiente_carta] =
                mano->cartas[i];
            partida->descartes.siguiente_carta += 1;
            mano->cartas[i] =
                partida->baraja.cartas[partida->baraja.siguiente_carta];
            partida->baraja.siguiente_carta += 1;
            if (partida->baraja.siguiente_carta == partida->baraja.tamano)
                if (barajarDescartes(partida))
                    return 1;
        }
    }
    return 0;
}

int puntuarRonda(PartidaMus *partida, int ganador, int tantos) {
    if (ganador == 0 || ganador == 2) {
        partida->tantos[0] += tantos;
        if (partida->tantos[0] >= 40)
            return 1;
    } else {
        partida->tantos[1] += tantos;
        if (partida->tantos[1] >= 40)
            return 2;
    }
    return 0;
}

int resetearMazo(PartidaMus *partida) {
    if (partida == NULL)
        return 1;
    barajar(&(partida->baraja));
    partida->baraja.siguiente_carta = 0;
    return 0;
}
