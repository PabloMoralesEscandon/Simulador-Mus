#include <stdint.h>

#include "baraja_espanola.h"
#include "mus_log.h"
#include "mus_sim.h"

/** Cartas por clase en la baraja de 40: 8 pitos, 4 de cada número
 *  intermedio y 8 cerdos. */
const ConteoMus BARAJA_MUS_COMPLETA = {.c = {8, 4, 4, 4, 4, 4, 4, 8}};

// Carta representativa de cada clase (el palo es irrelevante en el mus)
static const int NUMERO_ESPANOL_DESDE_MUS[CERDO + 1] = {
    AS, CUATRO, CINCO, SEIS, SIETE, SOTA, CABALLO, REY};

int manoCumpleCondicion(Mano mano, Condicion condicion) {
    if (mano.cartas == NULL || mano.tamano != TAMANO_MANO_MUS)
        return -1;
    for (size_t i = 0; i < mano.tamano; i++)
        if (valorMus(mano.cartas[i]) < 0)
            return -1;
    switch (condicion) {
    case NADA:
        return 1;
    case TIENE_JUEGO:
        return tieneJuego(mano);
    case TIENE_31:
        return sumaMano(mano) == 31;
    case PAR_Y_JUEGO: {
        int pares = tipoPares(mano);
        int juego = tieneJuego(mano);
        if (pares < 0 || juego < 0)
            return -1;
        return pares != NO_PAR && juego;
    }
    default:
        return -1;
    }
}

int jugarFaseMus(PartidaMus *partida,
                 const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]) {
    if (partida == NULL || estrategias == NULL || partida->mano < 0 ||
        (partida->numeroJugadores != MUS_DOS_JUGADORES &&
         partida->numeroJugadores != MUS_CUATRO_JUGADORES) ||
        partida->mano >= partida->numeroJugadores)
        return 1;
    for (int jugador = 0; jugador < partida->numeroJugadores; jugador++)
        if (estrategias[jugador].decidirMus == NULL ||
            estrategias[jugador].elegirDescartes == NULL)
            return 1;

    for (int rondaMus = 0; rondaMus < MAXIMO_RONDAS_FASE_MUS; rondaMus++) {
        int decisiones[NUMERO_JUGADORES_MUS] = {0};
        for (int turno = 0; turno < partida->numeroJugadores; turno++) {
            int jugador =
                (partida->mano + turno) % partida->numeroJugadores;
            decisiones[jugador] = estrategias[jugador].decidirMus(
                &partida->manos[jugador], jugador, partida->mano,
                partida->tantos, estrategias[jugador].contexto);
            if (decisiones[jugador] == 0)
                return 0;
            if (decisiones[jugador] != 1)
                return 1;
        }
        if (todosDanMusConJugadores(decisiones, partida->numeroJugadores) != 1)
            return 1;

        int descartadas[NUMERO_JUGADORES_MUS][TAMANO_MANO_MUS] = {{0}};
        for (int turno = 0; turno < partida->numeroJugadores; turno++) {
            int jugador =
                (partida->mano + turno) % partida->numeroJugadores;
            if (estrategias[jugador].elegirDescartes(
                    &partida->manos[jugador], jugador, descartadas[jugador],
                    estrategias[jugador].contexto))
                return 1;
            int numeroDescartes = 0;
            for (int carta = 0; carta < TAMANO_MANO_MUS; carta++) {
                if (descartadas[jugador][carta] != 0 &&
                    descartadas[jugador][carta] != 1)
                    return 1;
                numeroDescartes += descartadas[jugador][carta];
            }
            if (numeroDescartes == 0)
                return 1;
        }
        if (descartarManosMus(partida, descartadas))
            return 1;
    }
    return 1;
}

static int puedeApostarLance(const PartidaMus *partida, int jugador,
                             Ronda ronda) {
    if (ronda == PARES) {
        int tipo = tipoPares(partida->manos[jugador]);
        return tipo < 0 ? -1 : tipo != NO_PAR;
    }
    if (ronda == JUEGO)
        return tieneJuego(partida->manos[jugador]);
    return 1;
}

int jugarLanceEnvite(
    PartidaMus *partida, Ronda ronda,
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS],
    EnviteMus *resultado) {
    if (partida == NULL || estrategias == NULL || resultado == NULL ||
        partida->mano < 0 ||
        (partida->numeroJugadores != MUS_DOS_JUGADORES &&
         partida->numeroJugadores != MUS_CUATRO_JUGADORES) ||
        partida->mano >= partida->numeroJugadores ||
        ronda < GRANDE || ronda > PUNTO)
        return -1;
    for (int jugador = 0; jugador < partida->numeroJugadores; jugador++)
        if (estrategias[jugador].decidirEnvite == NULL)
            return -1;

    int pareja0 = 1;
    int pareja1 = 1;
    if (ronda == PARES) {
        pareja0 =
            equipoTienePares(partida->manos, partida->numeroJugadores, 0);
        pareja1 =
            equipoTienePares(partida->manos, partida->numeroJugadores, 1);
    } else if (ronda == JUEGO) {
        pareja0 =
            equipoTieneJuego(partida->manos, partida->numeroJugadores, 0);
        pareja1 =
            equipoTieneJuego(partida->manos, partida->numeroJugadores, 1);
    } else if (ronda == PUNTO &&
               (equipoTieneJuego(partida->manos, partida->numeroJugadores,
                                  0) ||
                equipoTieneJuego(partida->manos, partida->numeroJugadores,
                                  1))) {
        return -1;
    }
    if (pareja0 < 0 || pareja1 < 0)
        return -1;

    if (iniciarEnviteMus(resultado))
        return -1;
    if (!pareja0 || !pareja1)
        return registrarEnviteMus(partida, ronda, resultado);

    int elegibles = 0;
    for (int jugador = 0; jugador < partida->numeroJugadores; jugador++) {
        int elegible = puedeApostarLance(partida, jugador, ronda);
        if (elegible < 0)
            return -1;
        elegibles += elegible;
    }

    int jugador = partida->mano;
    int pasos = 0;
    for (;;) {
        int elegible = puedeApostarLance(partida, jugador, ronda);
        if (elegible < 0)
            return -1;
        if (!elegible ||
            (resultado->estado != ENVITE_AL_PASO &&
             jugador % 2 == resultado->parejaApostadora)) {
            jugador = (jugador + 1) % partida->numeroJugadores;
            continue;
        }

        AccionEnviteMus accion = estrategias[jugador].decidirEnvite(
            &partida->manos[jugador], jugador, partida->mano,
            partida->tantos, ronda, resultado,
            estrategias[jugador].contexto);
        if (accion.tipo == ACCION_PASAR &&
            resultado->estado == ENVITE_AL_PASO) {
            pasos++;
            if (pasos == elegibles)
                return registrarEnviteMus(partida, ronda, resultado);
        } else {
            if (aplicarAccionEnviteMus(resultado, jugador % 2, accion))
                return -1;
            pasos = 0;
            if (resultado->estado == ORDAGO_ACEPTADO)
                return resolverOrdagoMus(partida, ronda, resultado);
            if (resultado->estado == ENVITE_ACEPTADO ||
                resultado->estado == ENVITE_RECHAZADO ||
                resultado->estado == ORDAGO_RECHAZADO)
                return registrarEnviteMus(partida, ronda, resultado);
        }
        jugador = (jugador + 1) % partida->numeroJugadores;
    }
}

int simularRondaMusConEstrategias(
    PartidaMus *partida,
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]) {
    if (partida == NULL || estrategias == NULL || partida->mano < 0 ||
        (partida->numeroJugadores != MUS_DOS_JUGADORES &&
         partida->numeroJugadores != MUS_CUATRO_JUGADORES) ||
        partida->mano >= partida->numeroJugadores)
        return -1;
    for (int jugador = 0; jugador < partida->numeroJugadores; jugador++)
        if (estrategias[jugador].decidirMus == NULL ||
            estrategias[jugador].elegirDescartes == NULL ||
            estrategias[jugador].decidirEnvite == NULL)
            return -1;
    if (resetearMazo(partida))
        return -1;
    if (reiniciarEnvitesRonda(&partida->envites_actuales))
        return -1;
    if (repartirManos(partida))
        return -1;
    if (jugarFaseMus(partida, estrategias))
        return -1;
    logManos(LOG_LANCES, partida);
    EnviteMus envite;
    int resultado = jugarLanceEnvite(partida, GRANDE, estrategias, &envite);
    if (resultado)
        return resultado;
    if (envite.estado == ENVITE_RECHAZADO ||
        envite.estado == ORDAGO_RECHAZADO) {
        logGanadorLancePartida(LOG_LANCES, "Grande", partida,
                               envite.parejaApostadora);
    } else {
        logGanadorLancePartida(
            LOG_LANCES, "Grande", partida,
            ganadorGrandeConJugadores(partida->manos,
                                       partida->numeroJugadores,
                                       partida->mano));
        resultado = puntuarGrande(partida);
        if (resultado)
            return resultado;
    }

    resultado = jugarLanceEnvite(partida, CHICA, estrategias, &envite);
    if (resultado)
        return resultado;
    if (envite.estado == ENVITE_RECHAZADO ||
        envite.estado == ORDAGO_RECHAZADO) {
        logGanadorLancePartida(LOG_LANCES, "Chica", partida,
                               envite.parejaApostadora);
    } else {
        logGanadorLancePartida(
            LOG_LANCES, "Chica", partida,
            ganadorChicaConJugadores(partida->manos,
                                      partida->numeroJugadores,
                                      partida->mano));
        resultado = puntuarChica(partida);
        if (resultado)
            return resultado;
    }

    resultado = jugarLanceEnvite(partida, PARES, estrategias, &envite);
    if (resultado)
        return resultado;
    if (envite.estado == ENVITE_RECHAZADO ||
        envite.estado == ORDAGO_RECHAZADO) {
        logGanadorLancePartida(LOG_LANCES, "Pares", partida,
                               envite.parejaApostadora);
        resultado = puntuarParesDePareja(partida, envite.parejaApostadora, 0);
    } else {
        if (equipoTienePares(partida->manos, partida->numeroJugadores, 0) ||
            equipoTienePares(partida->manos, partida->numeroJugadores, 1))
            logGanadorLancePartida(
                LOG_LANCES, "Pares", partida,
                ganadorParConJugadores(partida->manos,
                                        partida->numeroJugadores,
                                        partida->mano));
        resultado = puntuarPares(partida);
    }
    if (resultado)
        return resultado;

    Ronda ultimaRonda = equipoTieneJuego(partida->manos,
                                         partida->numeroJugadores, 0) ||
                                equipoTieneJuego(partida->manos,
                                                 partida->numeroJugadores, 1)
                            ? JUEGO
                            : PUNTO;
    resultado = jugarLanceEnvite(partida, ultimaRonda, estrategias, &envite);
    if (resultado)
        return resultado;
    if (envite.estado == ENVITE_RECHAZADO ||
        envite.estado == ORDAGO_RECHAZADO) {
        logGanadorLancePartida(LOG_LANCES,
                               ultimaRonda == JUEGO ? "Juego" : "Punto",
                               partida, envite.parejaApostadora);
        resultado = puntuarJuegoOPuntoDePareja(
            partida, ultimaRonda, envite.parejaApostadora, 0);
    } else {
        if (ultimaRonda == JUEGO)
            logGanadorLancePartida(
                LOG_LANCES, "Juego", partida,
                ganadorJuegoConJugadores(partida->manos,
                                          partida->numeroJugadores,
                                          partida->mano));
        else
            logGanadorLancePartida(
                LOG_LANCES, "Punto", partida,
                ganadorPuntoConJugadores(partida->manos,
                                          partida->numeroJugadores,
                                          partida->mano));
        resultado = puntuarJuegoOPunto(partida);
    }
    if (resultado)
        return resultado;

    logTantos(LOG_RONDAS, partida);
    partida->mano = (partida->mano + 1) % partida->numeroJugadores;
    return 0;
}

static int cortarMus(const Mano *mano, int jugador, int manoPartida,
                     const int tantos[2], void *contexto) {
    (void)mano;
    (void)jugador;
    (void)manoPartida;
    (void)tantos;
    (void)contexto;
    return 0;
}

static int sinDescartes(const Mano *mano, int jugador,
                        int descartadas[TAMANO_MANO_MUS], void *contexto) {
    (void)mano;
    (void)jugador;
    (void)descartadas;
    (void)contexto;
    return 0;
}

static AccionEnviteMus pasarEnvite(
    const Mano *mano, int jugador, int manoPartida, const int tantos[2],
    Ronda ronda, const EnviteMus *envite, void *contexto) {
    (void)mano;
    (void)jugador;
    (void)manoPartida;
    (void)tantos;
    (void)ronda;
    (void)envite;
    (void)contexto;
    return (AccionEnviteMus){.tipo = ACCION_PASAR};
}

int simularRondaMus(PartidaMus *partida) {
    EstrategiaMus estrategias[NUMERO_JUGADORES_MUS] = {{0}};
    for (int jugador = 0; jugador < NUMERO_JUGADORES_MUS; jugador++) {
        estrategias[jugador].decidirMus = cortarMus;
        estrategias[jugador].elegirDescartes = sinDescartes;
        estrategias[jugador].decidirEnvite = pasarEnvite;
    }
    return simularRondaMusConEstrategias(partida, estrategias);
}

int simularPartidaMusConEstrategiasYJugadores(
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS],
    int numeroJugadores) {
    if (estrategias == NULL ||
        (numeroJugadores != MUS_DOS_JUGADORES &&
         numeroJugadores != MUS_CUATRO_JUGADORES))
        return 1;
    PartidaMus partida = {0};
    if (iniciarPartidaMusConJugadores(&partida, numeroJugadores))
        return 1;
    int ronda = 0;
    int ganador = 0;
    do {
        ronda += 1;
        logNumeroRonda(LOG_RONDAS, ronda);
    } while (!(ganador =
                   simularRondaMusConEstrategias(&partida, estrategias)));
    int resultado = 0;
    if (ganador == 1 || ganador == 2) {
        logGanadorPartida(LOG_RESULTADO, &partida);
    } else {
        resultado = 1;
    }
    destruirPartidaMus(&partida);
    return resultado;
}

int simularPartidaMusConEstrategias(
    const EstrategiaMus estrategias[NUMERO_JUGADORES_MUS]) {
    return simularPartidaMusConEstrategiasYJugadores(
        estrategias, MUS_CUATRO_JUGADORES);
}

int simularPartidaMusConJugadores(int numeroJugadores) {
    if (numeroJugadores != MUS_DOS_JUGADORES &&
        numeroJugadores != MUS_CUATRO_JUGADORES)
        return 1;
    EstrategiaMus estrategias[NUMERO_JUGADORES_MUS] = {{0}};
    for (int jugador = 0; jugador < numeroJugadores; jugador++) {
        estrategias[jugador].decidirMus = cortarMus;
        estrategias[jugador].elegirDescartes = sinDescartes;
        estrategias[jugador].decidirEnvite = pasarEnvite;
    }
    return simularPartidaMusConEstrategiasYJugadores(estrategias,
                                                      numeroJugadores);
}

int simularPartidaMus(void) {
    return simularPartidaMusConJugadores(MUS_CUATRO_JUGADORES);
}

/** Rellena mano con las cartas representativas de esas clases de valor. */
static void construirMano(Mano *mano, const int valores[TAMANO_MANO_MUS]) {
    for (size_t i = 0; i < TAMANO_MANO_MUS; i++) {
        mano->cartas[i].numero = NUMERO_ESPANOL_DESDE_MUS[valores[i]];
        mano->cartas[i].palo = OROS;
    }
}

/** Número combinatorio C(n, k). */
static uint64_t combinaciones(int n, int k) {
    if (k < 0 || k > n)
        return 0;

    uint64_t resultado = 1;
    // Tras cada paso resultado = C(n-k+i, i), así que la división es exacta
    for (int i = 1; i <= k; i++)
        resultado = resultado * (n - k + i) / i;
    return resultado;
}

static int numeroValidoMus(int numero) {
    switch (numero) {
    case AS:
    case DOS:
    case TRES:
    case CUATRO:
    case CINCO:
    case SEIS:
    case SIETE:
    case SOTA:
    case CABALLO:
    case REY:
        return 1;
    default:
        return 0;
    }
}

static int manosFijasValidas(Mano manos[], int numeroManos) {
    if (manos == NULL || numeroManos < 1 || numeroManos > 2)
        return 0;
    int vistas[BASTOS + 1][REY + 1] = {{0}};
    for (int jugador = 0; jugador < numeroManos; jugador++) {
        if (manos[jugador].cartas == NULL ||
            manos[jugador].tamano != TAMANO_MANO_MUS)
            return 0;
        for (size_t i = 0; i < manos[jugador].tamano; i++) {
            Carta carta = manos[jugador].cartas[i];
            if (!numeroValidoMus(carta.numero) || carta.palo < OROS ||
                carta.palo > BASTOS || vistas[carta.palo][carta.numero])
                return 0;
            vistas[carta.palo][carta.numero] = 1;
        }
    }
    return 1;
}

static int ganaParejaFija(Mano manos[NUMERO_JUGADORES_MUS], int mano,
                          Ronda ronda) {
    int ganador = -1;
    switch (ronda) {
    case GRANDE:
        ganador = ganadorGrande(manos, mano);
        break;
    case CHICA:
        ganador = ganadorChica(manos, mano);
        break;
    case PARES:
        if (!parejaTienePares(manos, 0) && !parejaTienePares(manos, 1))
            return 0;
        ganador = ganadorPar(manos, mano);
        break;
    case JUEGO:
        if (!parejaTieneJuego(manos, 0) && !parejaTieneJuego(manos, 1))
            return 0;
        ganador = ganadorJuego(manos, mano);
        break;
    case PUNTO:
        if (parejaTieneJuego(manos, 0) || parejaTieneJuego(manos, 1))
            return 0;
        ganador = ganadorPunto(manos, mano);
        break;
    }
    return ganador == 0 || ganador == 2;
}

/** Peso físico de una composición de cuatro cartas. */
static uint64_t pesoUnaMano(
    const ConteoMus *disponibles,
    const int valores[TAMANO_MANO_MUS]) {
    int usadas[CERDO + 1] = {0};
    for (size_t i = 0; i < TAMANO_MANO_MUS; i++)
        usadas[valores[i]] += 1;
    uint64_t peso = 1;
    for (int valor = PITO; valor <= CERDO; valor++)
        peso *= combinaciones(disponibles->c[valor], usadas[valor]);
    return peso;
}

static int ganaJugadorFijo(Mano manos[2], int mano, Ronda ronda) {
    int ganador = -1;
    switch (ronda) {
    case GRANDE:
        ganador = ganadorGrandeConJugadores(manos, 2, mano);
        break;
    case CHICA:
        ganador = ganadorChicaConJugadores(manos, 2, mano);
        break;
    case PARES:
        if (!equipoTienePares(manos, 2, 0) &&
            !equipoTienePares(manos, 2, 1))
            return 0;
        ganador = ganadorParConJugadores(manos, 2, mano);
        break;
    case JUEGO:
        if (!equipoTieneJuego(manos, 2, 0) &&
            !equipoTieneJuego(manos, 2, 1))
            return 0;
        ganador = ganadorJuegoConJugadores(manos, 2, mano);
        break;
    case PUNTO:
        if (equipoTieneJuego(manos, 2, 0) ||
            equipoTieneJuego(manos, 2, 1))
            return 0;
        ganador = ganadorPuntoConJugadores(manos, 2, mano);
        break;
    }
    return ganador == 0;
}

double probabilidadesVictoria1Fija(Mano mano, int manoPartida, Ronda ronda,
                                   Condicion condicionRival) {
    if (!manosFijasValidas(&mano, 1) || manoPartida < 0 || manoPartida >= 2 ||
        ronda < GRANDE || ronda > PUNTO || condicionRival < NADA ||
        condicionRival > PAR_Y_JUEGO)
        return -1.0;

    ConteoMus conteo = BARAJA_MUS_COMPLETA;
    for (size_t i = 0; i < mano.tamano; i++)
        conteo.c[valorMus(mano.cartas[i])] -= 1;

    Carta cartasRival[TAMANO_MANO_MUS];
    Mano rival = {.cartas = cartasRival, .tamano = TAMANO_MANO_MUS};
    uint64_t casos = 0;
    uint64_t exitos = 0;
    for (int carta1 = PITO; carta1 <= CERDO; carta1++) {
        ConteoMus temp = conteo;
        if (temp.c[carta1] == 0)
            continue;
        temp.c[carta1]--;
        for (int carta2 = carta1; carta2 <= CERDO; carta2++) {
            if (temp.c[carta2] == 0)
                continue;
            temp.c[carta2]--;
            for (int carta3 = carta2; carta3 <= CERDO; carta3++) {
                if (temp.c[carta3] == 0)
                    continue;
                temp.c[carta3]--;
                for (int carta4 = carta3; carta4 <= CERDO; carta4++) {
                    if (temp.c[carta4] == 0)
                        continue;
                    int valores[TAMANO_MANO_MUS] = {carta1, carta2, carta3,
                                                   carta4};
                    construirMano(&rival, valores);
                    if (manoCumpleCondicion(rival, condicionRival) == 1) {
                        uint64_t peso = pesoUnaMano(&conteo, valores);
                        Mano manosPrueba[2] = {mano, rival};
                        if (ganaJugadorFijo(manosPrueba, manoPartida, ronda))
                            exitos += peso;
                        casos += peso;
                    }
                }
                temp.c[carta3]++;
            }
            temp.c[carta2]++;
        }
    }
    if (casos == 0)
        return -1.0;
    return (double)exitos / (double)casos;
}

double probabilidadesVictoria2Fija(Mano manos[NUMERO_JUGADORES_MUS - 2],
                                   int mano, Ronda ronda,
                                   Condicion condicionMano1,
                                   Condicion condicionMano2) {
    if (!manosFijasValidas(manos, 2) || mano < 0 ||
        mano >= NUMERO_JUGADORES_MUS || ronda < GRANDE || ronda > PUNTO ||
        condicionMano1 < NADA || condicionMano1 > PAR_Y_JUEGO ||
        condicionMano2 < NADA || condicionMano2 > PAR_Y_JUEGO)
        return -1.0;
    // Retira de la baraja las cartas de las dos manos fijas
    ConteoMus conteo = BARAJA_MUS_COMPLETA;
    for (size_t m = 0; m < 2; m++)
        for (size_t i = 0; i < manos[m].tamano; i++)
            conteo.c[valorMus(manos[m].cartas[i])] -= 1;

    Carta cartasMano2[TAMANO_MANO_MUS];
    Mano mano2 = {.cartas = cartasMano2, .tamano = TAMANO_MANO_MUS};
    Carta cartasMano4[TAMANO_MANO_MUS];
    Mano mano4 = {.cartas = cartasMano4, .tamano = TAMANO_MANO_MUS};

    // Los acumuladores cuentan repartos físicos, no solo composiciones
    uint64_t casos = 0;
    uint64_t exitos = 0;
    // Enumera cada mano rival como multiconjunto ordenado (carta1 <= ... <=
    // carta4), de modo que cada composición aparece una sola vez; temp
    // comprueba que queden cartas y pesoUnaMano cuenta las elecciones físicas
    for (int carta1 = PITO; carta1 <= CERDO; carta1++) {
        ConteoMus temp = conteo;
        if (temp.c[carta1] == 0)
            continue;
        temp.c[carta1] -= 1;
        for (int carta2 = carta1; carta2 <= CERDO; carta2++) {
            if (temp.c[carta2] == 0)
                continue;
            temp.c[carta2] -= 1;
            for (int carta3 = carta2; carta3 <= CERDO; carta3++) {
                if (temp.c[carta3] == 0)
                    continue;
                temp.c[carta3] -= 1;
                for (int carta4 = carta3; carta4 <= CERDO; carta4++) {
                    if (temp.c[carta4] == 0)
                        continue;
                    temp.c[carta4] -= 1;

                    int valoresMano2[TAMANO_MANO_MUS] = {
                        carta1, carta2, carta3, carta4};
                    construirMano(&mano2, valoresMano2);
                    if (manoCumpleCondicion(mano2, condicionMano1) != 1) {
                        temp.c[carta4] += 1;
                        continue;
                    }
                    uint64_t pesoMano2 = pesoUnaMano(&conteo, valoresMano2);
                    ConteoMus disponiblesMano4 = temp;

                    for (int carta5 = PITO; carta5 <= CERDO; carta5++) {
                        if (temp.c[carta5] == 0)
                            continue;
                        temp.c[carta5] -= 1;
                        for (int carta6 = carta5; carta6 <= CERDO; carta6++) {
                            if (temp.c[carta6] == 0)
                                continue;
                            temp.c[carta6] -= 1;
                            for (int carta7 = carta6; carta7 <= CERDO;
                                 carta7++) {
                                if (temp.c[carta7] == 0)
                                    continue;
                                temp.c[carta7] -= 1;
                                for (int carta8 = carta7; carta8 <= CERDO;
                                     carta8++) {
                                    if (temp.c[carta8] == 0)
                                        continue;
                                    temp.c[carta8] -= 1;

                                    int valoresMano4[TAMANO_MANO_MUS] = {
                                        carta5, carta6, carta7, carta8};
                                    construirMano(&mano4, valoresMano4);
                                    if (manoCumpleCondicion(
                                            mano4, condicionMano2) != 1) {
                                        temp.c[carta8] += 1;
                                        continue;
                                    }
                                    uint64_t peso =
                                        pesoMano2 * pesoUnaMano(
                                                        &disponiblesMano4,
                                                        valoresMano4);
                                    // Las manos fijas ocupan las
                                    // posiciones 0 y 2 (su pareja)
                                    Mano manos_prueba[4] = {manos[0], mano2,
                                                            manos[1], mano4};
                                    if (ganaParejaFija(manos_prueba, mano,
                                                      ronda))
                                        exitos += peso;
                                    casos += peso;
                                    temp.c[carta8] += 1;
                                }
                                temp.c[carta7] += 1;
                            }
                            temp.c[carta6] += 1;
                        }
                        temp.c[carta5] += 1;
                    }
                    temp.c[carta4] += 1;
                }
                temp.c[carta3] += 1;
            }
            temp.c[carta2] += 1;
        }
        temp.c[carta1] += 1;
    }
    if (casos == 0)
        return -1.0;
    return (double)exitos / (double)casos;
}
