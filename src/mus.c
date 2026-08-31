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

static int puntuarLanceSimple(
    PartidaMus *partida, int envite,
    int (*ganadorLance)(Mano[NUMERO_JUGADORES_MUS], int)) {
    if (partida == NULL || ganadorLance == NULL || envite < 0)
        return -1;
    int ganador = ganadorLance(partida->manos, partida->mano);
    int tantos = envite == 0 ? 1 : envite;
    return puntuarRonda(partida, ganador, tantos);
}

int puntuarGrande(PartidaMus *partida) {
    if (partida == NULL)
        return -1;
    return puntuarLanceSimple(partida, partida->envites_actuales.grande,
                              ganadorGrande);
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

int puntuarChica(PartidaMus *partida) {
    if (partida == NULL)
        return -1;
    return puntuarLanceSimple(partida, partida->envites_actuales.chica,
                              ganadorChica);
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

int tipoPares(Mano mano) {
    int clave = clavePar(mano);
    if (clave >= 100)
        return DUPLEX;
    if (clave >= 10)
        return MEDIAS;
    if (clave > 0)
        return PAR;
    return NO_PAR;
}

int tantosPares(Mano mano) {
    switch (tipoPares(mano)) {
    case PAR:
        return 1;
    case MEDIAS:
        return 2;
    case DUPLEX:
        return 3;
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

int parejaTienePares(Mano manos[NUMERO_JUGADORES_MUS], int pareja) {
    if (manos == NULL || pareja < 0 || pareja > 1)
        return -1;
    return tipoPares(manos[pareja]) != NO_PAR ||
           tipoPares(manos[pareja + 2]) != NO_PAR;
}

int puntuarPares(PartidaMus *partida) {
    if (partida == NULL || partida->envites_actuales.pares < 0)
        return -1;

    int pares0 = parejaTienePares(partida->manos, 0);
    int pares1 = parejaTienePares(partida->manos, 1);
    if (!pares0 && !pares1)
        return partida->envites_actuales.pares == 0 ? 0 : -1;
    if ((!pares0 || !pares1) && partida->envites_actuales.pares != 0)
        return -1;

    int ganador = ganadorPar(partida->manos, partida->mano);
    int pareja = ganador % 2;
    int tantos = tantosPares(partida->manos[pareja]) +
                 tantosPares(partida->manos[pareja + 2]) +
                 partida->envites_actuales.pares;
    return puntuarRonda(partida, ganador, tantos);
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

int tieneJuego(Mano mano) { return sumaMano(mano) >= 31; }

int tantosJuego(Mano mano) {
    int suma = sumaMano(mano);
    if (suma == 31)
        return 3;
    if (suma > 31)
        return 2;
    return 0;
}

int parejaTieneJuego(Mano manos[NUMERO_JUGADORES_MUS], int pareja) {
    if (manos == NULL || pareja < 0 || pareja > 1)
        return -1;
    return tieneJuego(manos[pareja]) || tieneJuego(manos[pareja + 2]);
}

int puntuarJuegoOPunto(PartidaMus *partida) {
    if (partida == NULL || partida->envites_actuales.juego < 0 ||
        partida->envites_actuales.punto < 0)
        return -1;

    int juego0 = parejaTieneJuego(partida->manos, 0);
    int juego1 = parejaTieneJuego(partida->manos, 1);
    if (juego0 || juego1) {
        if (partida->envites_actuales.punto != 0)
            return -1;
        if ((!juego0 || !juego1) && partida->envites_actuales.juego != 0)
            return -1;
        int ganador = ganadorJuego(partida->manos, partida->mano);
        int pareja = ganador % 2;
        int tantos = tantosJuego(partida->manos[pareja]) +
                     tantosJuego(partida->manos[pareja + 2]) +
                     partida->envites_actuales.juego;
        return puntuarRonda(partida, ganador, tantos);
    }

    if (partida->envites_actuales.juego != 0)
        return -1;
    int ganador = ganadorPunto(partida->manos, partida->mano);
    return puntuarRonda(partida, ganador,
                        partida->envites_actuales.punto + 1);
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

int reiniciarEnvitesRonda(EnviteRonda *envites) {
    if (envites == NULL)
        return 1;
    *envites = (EnviteRonda){0};
    return 0;
}

int iniciarEnviteMus(EnviteMus *envite) {
    if (envite == NULL)
        return 1;
    *envite = (EnviteMus){.estado = ENVITE_AL_PASO,
                          .parejaApostadora = -1};
    return 0;
}

int envidarMus(EnviteMus *envite, int pareja, int cantidadTotal) {
    if (envite == NULL || pareja < 0 || pareja > 1 || cantidadTotal < 2)
        return 1;
    if (envite->estado == ENVITE_AL_PASO) {
        envite->cantidadAnterior = 1;
    } else if (envite->estado == ENVITE_PENDIENTE) {
        if (pareja == envite->parejaApostadora ||
            cantidadTotal <= envite->cantidad)
            return 1;
        envite->cantidadAnterior = envite->cantidad;
    } else {
        return 1;
    }
    envite->estado = ENVITE_PENDIENTE;
    envite->cantidad = cantidadTotal;
    envite->parejaApostadora = pareja;
    return 0;
}

int quererEnviteMus(EnviteMus *envite, int pareja) {
    if (envite == NULL || pareja < 0 || pareja > 1 ||
        pareja == envite->parejaApostadora)
        return 1;
    if (envite->estado == ENVITE_PENDIENTE)
        envite->estado = ENVITE_ACEPTADO;
    else if (envite->estado == ORDAGO_PENDIENTE)
        envite->estado = ORDAGO_ACEPTADO;
    else
        return 1;
    return 0;
}

int noQuererEnviteMus(EnviteMus *envite, int pareja) {
    if (envite == NULL || pareja < 0 || pareja > 1 ||
        pareja == envite->parejaApostadora)
        return 1;
    if (envite->estado == ENVITE_PENDIENTE)
        envite->estado = ENVITE_RECHAZADO;
    else if (envite->estado == ORDAGO_PENDIENTE)
        envite->estado = ORDAGO_RECHAZADO;
    else
        return 1;
    return 0;
}

int ordagoMus(EnviteMus *envite, int pareja) {
    if (envite == NULL || pareja < 0 || pareja > 1)
        return 1;
    if (envite->estado == ENVITE_AL_PASO) {
        envite->cantidadAnterior = 1;
    } else if (envite->estado == ENVITE_PENDIENTE) {
        if (pareja == envite->parejaApostadora)
            return 1;
        envite->cantidadAnterior = envite->cantidad;
    } else {
        return 1;
    }
    envite->estado = ORDAGO_PENDIENTE;
    envite->parejaApostadora = pareja;
    return 0;
}

int registrarEnviteMus(PartidaMus *partida, Ronda ronda,
                       const EnviteMus *envite) {
    if (partida == NULL || envite == NULL || ronda < GRANDE || ronda > PUNTO)
        return -1;

    int *cantidad = NULL;
    switch (ronda) {
    case GRANDE:
        cantidad = &partida->envites_actuales.grande;
        break;
    case CHICA:
        cantidad = &partida->envites_actuales.chica;
        break;
    case PARES:
        cantidad = &partida->envites_actuales.pares;
        break;
    case JUEGO:
        cantidad = &partida->envites_actuales.juego;
        break;
    case PUNTO:
        cantidad = &partida->envites_actuales.punto;
        break;
    }
    if (*cantidad != 0)
        return -1;

    if (envite->estado == ENVITE_AL_PASO)
        return 0;
    if (envite->estado == ENVITE_ACEPTADO) {
        if (envite->cantidad < 2)
            return -1;
        *cantidad = envite->cantidad;
        return 0;
    }
    if (envite->estado == ENVITE_RECHAZADO ||
        envite->estado == ORDAGO_RECHAZADO) {
        if (envite->parejaApostadora < 0 || envite->parejaApostadora > 1 ||
            envite->cantidadAnterior < 1)
            return -1;
        return puntuarRonda(partida, envite->parejaApostadora,
                            envite->cantidadAnterior);
    }
    return -1;
}

int resolverOrdagoMus(PartidaMus *partida, Ronda ronda,
                      const EnviteMus *envite) {
    if (partida == NULL || envite == NULL ||
        envite->estado != ORDAGO_ACEPTADO || ronda < GRANDE || ronda > PUNTO)
        return -1;

    int ganador = -1;
    switch (ronda) {
    case GRANDE:
        ganador = ganadorGrande(partida->manos, partida->mano);
        break;
    case CHICA:
        ganador = ganadorChica(partida->manos, partida->mano);
        break;
    case PARES:
        if (parejaTienePares(partida->manos, 0) != 1 ||
            parejaTienePares(partida->manos, 1) != 1)
            return -1;
        ganador = ganadorPar(partida->manos, partida->mano);
        break;
    case JUEGO:
        if (parejaTieneJuego(partida->manos, 0) != 1 ||
            parejaTieneJuego(partida->manos, 1) != 1)
            return -1;
        ganador = ganadorJuego(partida->manos, partida->mano);
        break;
    case PUNTO:
        if (parejaTieneJuego(partida->manos, 0) != 0 ||
            parejaTieneJuego(partida->manos, 1) != 0)
            return -1;
        ganador = ganadorPunto(partida->manos, partida->mano);
        break;
    }

    int pareja = ganador % 2;
    partida->tantos[pareja] = 40;
    reiniciarEnvitesRonda(&partida->envites_actuales);
    return pareja + 1;
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
    if (reiniciarEnvitesRonda(&partida->envites_actuales))
        return 1;
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
    reiniciarEnvitesRonda(&partida->envites_actuales);
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

int todosDanMus(const int decisiones[NUMERO_JUGADORES_MUS]) {
    if (decisiones == NULL)
        return -1;
    for (int jugador = 0; jugador < NUMERO_JUGADORES_MUS; jugador++) {
        if (decisiones[jugador] != 0 && decisiones[jugador] != 1)
            return -1;
        if (decisiones[jugador] == 0)
            return 0;
    }
    return 1;
}

int descartarManosMus(
    PartidaMus *partida,
    int descartadas[NUMERO_JUGADORES_MUS][TAMANO_MANO_MUS]) {
    if (partida == NULL || descartadas == NULL || partida->mano < 0 ||
        partida->mano >= NUMERO_JUGADORES_MUS)
        return 1;
    for (int jugador = 0; jugador < NUMERO_JUGADORES_MUS; jugador++)
        for (int carta = 0; carta < TAMANO_MANO_MUS; carta++)
            if (descartadas[jugador][carta] != 0 &&
                descartadas[jugador][carta] != 1)
                return 1;

    for (int turno = 0; turno < NUMERO_JUGADORES_MUS; turno++) {
        int jugador = (partida->mano + turno) % NUMERO_JUGADORES_MUS;
        if (manoSeDescarta(partida, &partida->manos[jugador],
                           descartadas[jugador]))
            return 1;
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
