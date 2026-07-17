#include <stdint.h>

#include "baraja_espanola.h"
#include "mus_sim.h"

const ConteoMus BARAJA_MUS_COMPLETA = {.c = {8, 4, 4, 4, 4, 4, 4, 8}};

static const int NUMERO_ESPANOL_DESDE_MUS[CERDO + 1] = {
    AS, CUATRO, CINCO, SEIS, SIETE, SOTA, CABALLO, REY};

int simularRondaMus(PartidaMus *partida) {
    if (partida == NULL)
        return -1;
    int ganador = 0;
    repartirManos(partida);
    ganador =
        puntuarRonda(partida, ganadorGrande(partida->manos, partida->mano), 1);
    if (ganador)
        return ganador;
    ganador =
        puntuarRonda(partida, ganadorChica(partida->manos, partida->mano), 1);
    if (ganador)
        return ganador;
    ganador =
        puntuarRonda(partida, ganadorPar(partida->manos, partida->mano), 1);
    if (ganador)
        return ganador;
    ganador =
        puntuarRonda(partida, ganadorJuego(partida->manos, partida->mano), 1);
    if (ganador)
        return ganador;
    partida->mano = (partida->mano + 1) % NUMERO_JUGADORES_MUS;
    return 0;
}

int simularPartidaMus() {
    PartidaMus partida = {0};
    if (iniciarPartidaMus(&partida))
        return 1;
    do {
        if (resetearMazo(&partida))
            return 1;
    } while (!simularRondaMus(&partida));
    if (partida.tantos[0] >= 40) {
        // Imprimir ganador
    } else if (partida.tantos[1] >= 40) {
        // Imprimir ganador
    } else {
        // Error
    }
    destruirPartidaMus(&partida);
    return 0;
}

static void construirMano(Mano *mano, const int valores[TAMANO_MANO_MUS]) {
    for (size_t i = 0; i < TAMANO_MANO_MUS; i++) {
        mano->cartas[i].numero = NUMERO_ESPANOL_DESDE_MUS[valores[i]];
        mano->cartas[i].palo = OROS;
    }
}

static uint64_t combinaciones(int n, int k) {
    if (k < 0 || k > n)
        return 0;

    uint64_t resultado = 1;
    for (int i = 1; i <= k; i++)
        resultado = resultado * (n - k + i) / i;
    return resultado;
}

static uint64_t pesoCombinatorio(const ConteoMus *disponibles,
                                 const int valoresMano2[TAMANO_MANO_MUS],
                                 const int valoresMano4[TAMANO_MANO_MUS]) {
    ConteoMus usadasMano2 = {0};
    ConteoMus usadasMano4 = {0};

    for (size_t i = 0; i < TAMANO_MANO_MUS; i++) {
        usadasMano2.c[valoresMano2[i]] += 1;
        usadasMano4.c[valoresMano4[i]] += 1;
    }

    uint64_t peso = 1;
    for (int valor = PITO; valor <= CERDO; valor++) {
        peso *= combinaciones(disponibles->c[valor], usadasMano2.c[valor]);
        peso *= combinaciones(disponibles->c[valor] - usadasMano2.c[valor],
                              usadasMano4.c[valor]);
    }
    return peso;
}

double probabilidadesVictoria2Fija(Mano manos[NUMERO_JUGADORES_MUS - 2],
                                   int mano, Ronda ronda,
                                   Condicion condicionMano1,
                                   Condicion condicionMano2) {
    ConteoMus conteo = BARAJA_MUS_COMPLETA;
    for (size_t mano = 0; mano < 2; mano++)
        for (size_t i = 0; i < manos[mano].tamano; i++)
            conteo.c[valorMus(manos[mano].cartas[i])] -= 1;

    Carta cartasMano2[TAMANO_MANO_MUS];
    Mano mano2 = {.cartas = cartasMano2, .tamano = TAMANO_MANO_MUS};
    Carta cartasMano4[TAMANO_MANO_MUS];
    Mano mano4 = {.cartas = cartasMano4, .tamano = TAMANO_MANO_MUS};

    /* Estos acumuladores cuentan repartos fisicos, no solo composiciones.
     */
    uint64_t casos = 0;
    uint64_t exitos = 0;
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

                                    int valoresMano2[TAMANO_MANO_MUS] = {
                                        carta1, carta2, carta3, carta4};
                                    int valoresMano4[TAMANO_MANO_MUS] = {
                                        carta5, carta6, carta7, carta8};
                                    uint64_t peso = pesoCombinatorio(
                                        &conteo, valoresMano2, valoresMano4);

                                    construirMano(&mano2, valoresMano2);
                                    construirMano(&mano4, valoresMano4);
                                    Mano manos_prueba[4] = {manos[0], mano2,
                                                            manos[1], mano4};
                                    int ganador =
                                        ganadorGrande(manos_prueba, mano);
                                    if (ganador == 0 || ganador == 2)
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
        return 0.0;
    return (double)exitos / (double)casos;
}
