#include <limits.h>
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

static int numeroJugadoresValido(int numeroJugadores) {
    return numeroJugadores == MUS_DOS_JUGADORES ||
           numeroJugadores == MUS_CUATRO_JUGADORES;
}

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
        return -1;
    }
}

int claveGrande(Mano mano) {
    int clave = 0;
    for (size_t i = 0; i < mano.tamano; i++) {
        int valor = valorMus(mano.cartas[i]);
        if (valor < PITO || valor > CERDO)
            return -1;
        clave += PESO_GRANDE[valor];
    }
    return clave;
}

int ganadorGrandeConJugadores(Mano manos[], int numeroJugadores, int mano) {
    if (manos == NULL || !numeroJugadoresValido(numeroJugadores) || mano < 0 ||
        mano >= numeroJugadores)
        return -1;
    int max = claveGrande(manos[mano]);
    if (max < 0)
        return -1;
    int ganador = mano;
    for (int i = 1; i < numeroJugadores; i++) {
        int j = (mano + i) % numeroJugadores;
        int valor = claveGrande(manos[j]);
        if (valor < 0)
            return -1;
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

int ganadorGrande(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    return ganadorGrandeConJugadores(manos, MUS_CUATRO_JUGADORES, mano);
}

static int puntuarLanceSimple(
    PartidaMus *partida, int envite,
    int (*ganadorLance)(Mano[], int, int)) {
    if (partida == NULL || ganadorLance == NULL || envite < 0 ||
        !numeroJugadoresValido(partida->numeroJugadores))
        return -1;
    int ganador = ganadorLance(partida->manos, partida->numeroJugadores,
                               partida->mano);
    int tantos = envite == 0 ? 1 : envite;
    return puntuarRonda(partida, ganador, tantos);
}

int puntuarGrande(PartidaMus *partida) {
    if (partida == NULL)
        return -1;
    return puntuarLanceSimple(partida, partida->envites_actuales.grande,
                              ganadorGrandeConJugadores);
}

int claveChica(Mano mano) {
    int clave = 0;
    for (size_t i = 0; i < mano.tamano; i++) {
        int valor = valorMus(mano.cartas[i]);
        if (valor < PITO || valor > CERDO)
            return -1;
        clave += PESO_CHICA[valor];
    }
    return clave;
}

int ganadorChicaConJugadores(Mano manos[], int numeroJugadores, int mano) {
    if (manos == NULL || !numeroJugadoresValido(numeroJugadores) || mano < 0 ||
        mano >= numeroJugadores)
        return -1;
    int max = claveChica(manos[mano]);
    if (max < 0)
        return -1;
    int ganador = mano;
    for (int i = 1; i < numeroJugadores; i++) {
        int j = (mano + i) % numeroJugadores;
        int valor = claveChica(manos[j]);
        if (valor < 0)
            return -1;
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

int ganadorChica(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    return ganadorChicaConJugadores(manos, MUS_CUATRO_JUGADORES, mano);
}

int puntuarChica(PartidaMus *partida) {
    if (partida == NULL)
        return -1;
    return puntuarLanceSimple(partida, partida->envites_actuales.chica,
                              ganadorChicaConJugadores);
}

int clavePar(Mano mano) {
    int c[CERDO + 1] = {0};
    int tipo = NO_PAR;
    int alto = 0, bajo = 0;
    // Cuenta cuántas cartas hay de cada clase
    for (size_t i = 0; i < mano.tamano; i++) {
        int valor = valorMus(mano.cartas[i]);
        if (valor < PITO || valor > CERDO)
            return -1;
        c[valor]++;
    }
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
    if (clave < 0)
        return -1;
    if (clave >= 100)
        return DUPLEX;
    if (clave >= 10)
        return MEDIAS;
    if (clave > 0)
        return PAR;
    return NO_PAR;
}

int tantosPares(Mano mano) {
    int tipo = tipoPares(mano);
    switch (tipo) {
    case PAR:
        return 1;
    case MEDIAS:
        return 2;
    case DUPLEX:
        return 3;
    default:
        return tipo == NO_PAR ? 0 : -1;
    }
}

int ganadorParConJugadores(Mano manos[], int numeroJugadores, int mano) {
    if (manos == NULL || !numeroJugadoresValido(numeroJugadores) || mano < 0 ||
        mano >= numeroJugadores)
        return -1;
    int max = clavePar(manos[mano]);
    if (max < 0)
        return -1;
    int ganador = mano;
    for (int i = 1; i < numeroJugadores; i++) {
        int j = (mano + i) % numeroJugadores;
        int valor = clavePar(manos[j]);
        if (valor < 0)
            return -1;
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

int ganadorPar(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    return ganadorParConJugadores(manos, MUS_CUATRO_JUGADORES, mano);
}

int equipoTienePares(Mano manos[], int numeroJugadores, int equipo) {
    if (manos == NULL || !numeroJugadoresValido(numeroJugadores) ||
        equipo < 0 || equipo > 1)
        return -1;
    for (int jugador = equipo; jugador < numeroJugadores; jugador += 2) {
        int tipo = tipoPares(manos[jugador]);
        if (tipo < 0)
            return -1;
        if (tipo != NO_PAR)
            return 1;
    }
    return 0;
}

int parejaTienePares(Mano manos[NUMERO_JUGADORES_MUS], int pareja) {
    return equipoTienePares(manos, MUS_CUATRO_JUGADORES, pareja);
}

int puntuarParesDePareja(PartidaMus *partida, int pareja, int tantosEnvite) {
    if (partida == NULL || !numeroJugadoresValido(partida->numeroJugadores) ||
        pareja < 0 || pareja > 1 || tantosEnvite < 0 ||
        equipoTienePares(partida->manos, partida->numeroJugadores, pareja) !=
            1)
        return -1;

    int tantos = 0;
    for (int jugador = pareja; jugador < partida->numeroJugadores;
         jugador += 2) {
        int tantosJugador = tantosPares(partida->manos[jugador]);
        if (tantosJugador < 0)
            return -1;
        tantos += tantosJugador;
    }
    tantos = tantosEnvite > INT_MAX - tantos ? INT_MAX : tantos + tantosEnvite;
    return puntuarRonda(partida, pareja, tantos);
}

int puntuarPares(PartidaMus *partida) {
    if (partida == NULL || partida->envites_actuales.pares < 0)
        return -1;

    if (!numeroJugadoresValido(partida->numeroJugadores))
        return -1;
    int pares0 = equipoTienePares(partida->manos, partida->numeroJugadores, 0);
    int pares1 = equipoTienePares(partida->manos, partida->numeroJugadores, 1);
    if (pares0 < 0 || pares1 < 0)
        return -1;
    if (!pares0 && !pares1)
        return partida->envites_actuales.pares == 0 ? 0 : -1;
    if ((!pares0 || !pares1) && partida->envites_actuales.pares != 0)
        return -1;

    int ganador = ganadorParConJugadores(partida->manos,
                                         partida->numeroJugadores,
                                         partida->mano);
    int pareja = ganador % 2;
    return puntuarParesDePareja(partida, pareja,
                                partida->envites_actuales.pares);
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
        int valor = valorPuntoMus(mano.cartas[i]);
        if (valor < 0)
            return -1;
        cuenta += valor;
    }
    return cuenta;
}

int tieneJuego(Mano mano) {
    int suma = sumaMano(mano);
    return suma < 0 ? -1 : suma >= 31;
}

int tantosJuego(Mano mano) {
    int suma = sumaMano(mano);
    if (suma < 0)
        return -1;
    if (suma == 31)
        return 3;
    if (suma > 31)
        return 2;
    return 0;
}

int equipoTieneJuego(Mano manos[], int numeroJugadores, int equipo) {
    if (manos == NULL || !numeroJugadoresValido(numeroJugadores) ||
        equipo < 0 || equipo > 1)
        return -1;
    for (int jugador = equipo; jugador < numeroJugadores; jugador += 2) {
        int juego = tieneJuego(manos[jugador]);
        if (juego < 0)
            return -1;
        if (juego)
            return 1;
    }
    return 0;
}

int parejaTieneJuego(Mano manos[NUMERO_JUGADORES_MUS], int pareja) {
    return equipoTieneJuego(manos, MUS_CUATRO_JUGADORES, pareja);
}

int puntuarJuegoOPuntoDePareja(PartidaMus *partida, Ronda ronda, int pareja,
                               int tantosEnvite) {
    if (partida == NULL || !numeroJugadoresValido(partida->numeroJugadores) ||
        pareja < 0 || pareja > 1 || tantosEnvite < 0)
        return -1;
    int tantos = 0;
    if (ronda == JUEGO) {
        if (equipoTieneJuego(partida->manos, partida->numeroJugadores,
                             pareja) != 1)
            return -1;
        for (int jugador = pareja; jugador < partida->numeroJugadores;
             jugador += 2) {
            int tantosJugador = tantosJuego(partida->manos[jugador]);
            if (tantosJugador < 0)
                return -1;
            tantos += tantosJugador;
        }
    } else if (ronda == PUNTO) {
        if (equipoTieneJuego(partida->manos, partida->numeroJugadores, 0) !=
                0 ||
            equipoTieneJuego(partida->manos, partida->numeroJugadores, 1) !=
                0)
            return -1;
        tantos = 1;
    } else {
        return -1;
    }
    tantos = tantosEnvite > INT_MAX - tantos ? INT_MAX : tantos + tantosEnvite;
    return puntuarRonda(partida, pareja, tantos);
}

int puntuarJuegoOPunto(PartidaMus *partida) {
    if (partida == NULL || partida->envites_actuales.juego < 0 ||
        partida->envites_actuales.punto < 0)
        return -1;

    if (!numeroJugadoresValido(partida->numeroJugadores))
        return -1;
    int juego0 = equipoTieneJuego(partida->manos, partida->numeroJugadores, 0);
    int juego1 = equipoTieneJuego(partida->manos, partida->numeroJugadores, 1);
    if (juego0 < 0 || juego1 < 0)
        return -1;
    if (juego0 || juego1) {
        if (partida->envites_actuales.punto != 0)
            return -1;
        if ((!juego0 || !juego1) && partida->envites_actuales.juego != 0)
            return -1;
        int ganador = ganadorJuegoConJugadores(
            partida->manos, partida->numeroJugadores, partida->mano);
        int pareja = ganador % 2;
        return puntuarJuegoOPuntoDePareja(
            partida, JUEGO, pareja, partida->envites_actuales.juego);
    }

    if (partida->envites_actuales.juego != 0)
        return -1;
    int ganador = ganadorPuntoConJugadores(
        partida->manos, partida->numeroJugadores, partida->mano);
    return puntuarJuegoOPuntoDePareja(
        partida, PUNTO, ganador % 2, partida->envites_actuales.punto);
}

/** Posición del juego en ORDEN_PUNTO (mayor es mejor); -1 sin juego. */
static int claveJuego(Mano mano) {
    int valor = sumaMano(mano);
    if (valor < 0)
        return -2;
    for (int i = 0; i < 8; i++)
        if (valor == ORDEN_PUNTO[i])
            return i;
    return -1;
}

int ganadorJuegoConJugadores(Mano manos[], int numeroJugadores, int mano) {
    if (manos == NULL || !numeroJugadoresValido(numeroJugadores) || mano < 0 ||
        mano >= numeroJugadores)
        return -1;
    int max = claveJuego(manos[mano]);
    if (max == -2)
        return -1;
    int ganador = mano;
    for (int i = 1; i < numeroJugadores; i++) {
        int j = (mano + i) % numeroJugadores;
        int valor = claveJuego(manos[j]);
        if (valor == -2)
            return -1;
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    if (max == -1)
        return ganadorPuntoConJugadores(manos, numeroJugadores, mano);
    return ganador;
}

int ganadorJuego(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    return ganadorJuegoConJugadores(manos, MUS_CUATRO_JUGADORES, mano);
}

int ganadorPuntoConJugadores(Mano manos[], int numeroJugadores, int mano) {
    if (manos == NULL || !numeroJugadoresValido(numeroJugadores) || mano < 0 ||
        mano >= numeroJugadores)
        return -1;
    int max = sumaMano(manos[mano]);
    if (max < 0)
        return -1;
    int ganador = mano;
    for (int i = 1; i < numeroJugadores; i++) {
        int j = (mano + i) % numeroJugadores;
        int valor = sumaMano(manos[j]);
        if (valor < 0)
            return -1;
        if (valor > max) {
            max = valor;
            ganador = j;
        }
    }
    return ganador;
}

int ganadorPunto(Mano manos[NUMERO_JUGADORES_MUS], int mano) {
    return ganadorPuntoConJugadores(manos, MUS_CUATRO_JUGADORES, mano);
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
        envite->estado != ORDAGO_ACEPTADO || ronda < GRANDE || ronda > PUNTO ||
        partida->mano < 0)
        return -1;
    if (!numeroJugadoresValido(partida->numeroJugadores) ||
        partida->mano >= partida->numeroJugadores)
        return -1;

    int ganador = -1;
    switch (ronda) {
    case GRANDE:
        ganador = ganadorGrandeConJugadores(
            partida->manos, partida->numeroJugadores, partida->mano);
        break;
    case CHICA:
        ganador = ganadorChicaConJugadores(
            partida->manos, partida->numeroJugadores, partida->mano);
        break;
    case PARES:
        if (equipoTienePares(partida->manos, partida->numeroJugadores, 0) !=
                1 ||
            equipoTienePares(partida->manos, partida->numeroJugadores, 1) !=
                1)
            return -1;
        ganador = ganadorParConJugadores(
            partida->manos, partida->numeroJugadores, partida->mano);
        break;
    case JUEGO:
        if (equipoTieneJuego(partida->manos, partida->numeroJugadores, 0) !=
                1 ||
            equipoTieneJuego(partida->manos, partida->numeroJugadores, 1) !=
                1)
            return -1;
        ganador = ganadorJuegoConJugadores(
            partida->manos, partida->numeroJugadores, partida->mano);
        break;
    case PUNTO:
        if (equipoTieneJuego(partida->manos, partida->numeroJugadores, 0) !=
                0 ||
            equipoTieneJuego(partida->manos, partida->numeroJugadores, 1) !=
                0)
            return -1;
        ganador = ganadorPuntoConJugadores(
            partida->manos, partida->numeroJugadores, partida->mano);
        break;
    }

    if (ganador < 0 || ganador >= partida->numeroJugadores)
        return -1;
    int pareja = ganador % 2;
    partida->tantos[pareja] = 40;
    reiniciarEnvitesRonda(&partida->envites_actuales);
    return pareja + 1;
}

int aplicarAccionEnviteMus(EnviteMus *envite, int pareja,
                           AccionEnviteMus accion) {
    switch (accion.tipo) {
    case ACCION_PASAR:
        if (envite == NULL)
            return 1;
        if (envite->estado == ENVITE_AL_PASO)
            return 0;
        return noQuererEnviteMus(envite, pareja);
    case ACCION_ENVIDAR:
        return envidarMus(envite, pareja, accion.cantidadTotal);
    case ACCION_QUERER:
        return quererEnviteMus(envite, pareja);
    case ACCION_NO_QUERER:
        return noQuererEnviteMus(envite, pareja);
    case ACCION_ORDAGO:
        return ordagoMus(envite, pareja);
    default:
        return 1;
    }
}

int iniciarPartidaMusConJugadores(PartidaMus *partida, int numeroJugadores) {
    if (partida == NULL || !numeroJugadoresValido(numeroJugadores))
        return 1;
    *partida = (PartidaMus){.numeroJugadores = numeroJugadores};
    if (crearBarajaEspanola40(&partida->baraja))
        return 1;
    if (crearBarajaEspanola40(&partida->descartes)) {
        destruirBaraja(&partida->baraja);
        return 1;
    }
    for (int jugador = 0; jugador < numeroJugadores; jugador++) {
        if (crearManoMus(&partida->manos[jugador])) {
            destruirPartidaMus(partida);
            return 1;
        }
    }
    return 0;
}

int iniciarPartidaMus(PartidaMus *partida) {
    return iniciarPartidaMusConJugadores(partida, MUS_CUATRO_JUGADORES);
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
    partida->numeroJugadores = 0;
    partida->mano = 0;
    reiniciarEnvitesRonda(&partida->envites_actuales);
    return 0;
}

int barajarDescartes(PartidaMus *partida) {
    if (partida == NULL || partida->baraja.cartas == NULL ||
        partida->descartes.cartas == NULL ||
        partida->descartes.siguiente_carta == 0 ||
        partida->descartes.siguiente_carta > partida->descartes.tamano)
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

static int sacarCartaReciclandoDescartes(PartidaMus *partida, Carta *carta) {
    if (partida == NULL || carta == NULL)
        return 1;
    if (partida->baraja.siguiente_carta >= partida->baraja.tamano &&
        barajarDescartes(partida))
        return 1;
    return sacarCarta(&partida->baraja, carta);
}

int repartirMano(PartidaMus *partida, Mano *mano) {
    if (partida == NULL || mano == NULL || mano->cartas == NULL)
        return 1;
    for (size_t i = 0; i < mano->tamano; i++)
        if (sacarCartaReciclandoDescartes(partida, &mano->cartas[i]))
            return 1;
    return 0;
}

int repartirManos(PartidaMus *partida) {
    if (partida == NULL || !numeroJugadoresValido(partida->numeroJugadores))
        return 1;
    for (int i = 0; i < partida->numeroJugadores; i++)
        if (repartirMano(partida, &(partida->manos[i])))
            return 1;
    return 0;
}

int manoSeDescarta(PartidaMus *partida, Mano *mano,
                   int descartadas[TAMANO_MANO_MUS]) {
    if (partida == NULL || mano == NULL || descartadas == NULL ||
        mano->cartas == NULL || mano->tamano != TAMANO_MANO_MUS ||
        partida->descartes.cartas == NULL)
        return 1;
    for (size_t i = 0; i < TAMANO_MANO_MUS; i++)
        if (descartadas[i] != 0 && descartadas[i] != 1)
            return 1;
    for (size_t i = 0; i < TAMANO_MANO_MUS; i++) {
        if (descartadas[i]) {
            // La carta descartada va a la pila y se roba una nueva;
            // si el mazo se agota, se recicla la pila de descartes
            if (partida->descartes.siguiente_carta >=
                partida->descartes.tamano)
                return 1;
            partida->descartes.cartas[partida->descartes.siguiente_carta] =
                mano->cartas[i];
            partida->descartes.siguiente_carta += 1;
            if (sacarCartaReciclandoDescartes(partida, &mano->cartas[i]))
                return 1;
        }
    }
    return 0;
}

int todosDanMus(const int decisiones[NUMERO_JUGADORES_MUS]) {
    return todosDanMusConJugadores(decisiones, MUS_CUATRO_JUGADORES);
}

int todosDanMusConJugadores(const int decisiones[], int numeroJugadores) {
    if (decisiones == NULL || !numeroJugadoresValido(numeroJugadores))
        return -1;
    for (int jugador = 0; jugador < numeroJugadores; jugador++) {
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
        !numeroJugadoresValido(partida->numeroJugadores) ||
        partida->mano >= partida->numeroJugadores)
        return 1;
    for (int jugador = 0; jugador < partida->numeroJugadores; jugador++)
        for (int carta = 0; carta < TAMANO_MANO_MUS; carta++)
            if (descartadas[jugador][carta] != 0 &&
                descartadas[jugador][carta] != 1)
                return 1;

    for (int turno = 0; turno < partida->numeroJugadores; turno++) {
        int jugador = (partida->mano + turno) % partida->numeroJugadores;
        if (manoSeDescarta(partida, &partida->manos[jugador],
                           descartadas[jugador]))
            return 1;
    }
    return 0;
}

int puntuarRonda(PartidaMus *partida, int ganador, int tantos) {
    if (partida == NULL || !numeroJugadoresValido(partida->numeroJugadores) ||
        ganador < 0 || ganador >= partida->numeroJugadores || tantos < 0 ||
        partida->tantos[0] < 0 || partida->tantos[0] > 40 ||
        partida->tantos[1] < 0 || partida->tantos[1] > 40)
        return -1;
    if (partida->tantos[0] == 40)
        return 1;
    if (partida->tantos[1] == 40)
        return 2;

    int pareja = ganador % 2;
    if (tantos >= 40 - partida->tantos[pareja]) {
        partida->tantos[pareja] = 40;
        return pareja + 1;
    }
    partida->tantos[pareja] += tantos;
    return 0;
}

int resetearMazo(PartidaMus *partida) {
    if (partida == NULL || partida->baraja.cartas == NULL ||
        partida->descartes.cartas == NULL)
        return 1;

    static const int numeros[] = {AS,   DOS,   TRES, CUATRO,  CINCO,
                                  SEIS, SIETE, SOTA, CABALLO, REY};
    for (int palo = OROS; palo <= BASTOS; palo++) {
        for (size_t i = 0; i < sizeof(numeros) / sizeof(numeros[0]); i++) {
            size_t posicion = (size_t)palo * 10 + i;
            partida->baraja.cartas[posicion] =
                (Carta){.numero = numeros[i], .palo = palo};
        }
    }
    partida->baraja.tamano = 40;
    partida->baraja.siguiente_carta = 0;
    partida->descartes.tamano = 40;
    partida->descartes.siguiente_carta = 0;
    return barajar(&partida->baraja);
}
