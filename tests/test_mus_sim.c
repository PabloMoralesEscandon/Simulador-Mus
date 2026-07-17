#include <math.h>
#include <stdlib.h>

#include "baraja_espanola.h"
#include "mus_io.h"
#include "mus_sim.h"
#include "utiles_test.h"

// Construye una mano de mus con los cuatro números dados
static Mano manoDe(int n1, int n2, int n3, int n4) {
    Mano mano;
    crearManoMus(&mano);
    int numeros[TAMANO_MANO_MUS] = {n1, n2, n3, n4};
    for (size_t i = 0; i < mano.tamano; i++) {
        mano.cartas[i].numero = numeros[i];
        mano.cartas[i].palo = (int)i;
    }
    return mano;
}

static void testSimularRondaMus(void) {
    VERIFICAR(simularRondaMus(NULL) == -1);

    PartidaMus partida = {0};
    VERIFICAR(iniciarPartidaMus(&partida) == 0);
    VERIFICAR(resetearMazo(&partida) == 0);
    VERIFICAR(simularRondaMus(&partida) == 0);
    // Una ronda reparte 4 lances de 1 tanto y pasa la mano al siguiente
    VERIFICAR(partida.tantos[0] + partida.tantos[1] == 4);
    VERIFICAR(partida.mano == 1);
    for (int jugador = 0; jugador < NUMERO_JUGADORES_MUS; jugador++)
        VERIFICAR(partida.manos[jugador].tamano == TAMANO_MANO_MUS);
    VERIFICAR(destruirPartidaMus(&partida) == 0);
}

static void testPartidaPorRondas(void) {
    PartidaMus partida = {0};
    VERIFICAR(iniciarPartidaMus(&partida) == 0);
    int ganador = 0;
    int rondas = 0;
    do {
        VERIFICAR(resetearMazo(&partida) == 0);
        ganador = simularRondaMus(&partida);
        rondas += 1;
    } while (ganador == 0 && rondas < 100);
    VERIFICAR(ganador == 1 || ganador == 2);
    VERIFICAR(partida.tantos[ganador - 1] >= 40);
    VERIFICAR(partida.tantos[2 - ganador] < 40);
    // Con 4 tantos por ronda hacen falta al menos 10 rondas para llegar a 40
    VERIFICAR(rondas >= 10);
    VERIFICAR(destruirPartidaMus(&partida) == 0);
}

static void testSimularPartidaMus(void) {
    for (int i = 0; i < 10; i++)
        VERIFICAR(simularPartidaMus() == 0);
}

static void testProbabilidadesCasosSeguros(void) {
    // Cuatro reyes siendo mano ganan la grande siempre (los empates con
    // cuatro treses los resuelve la posición)
    Mano seguras[2] = {manoDe(REY, REY, REY, REY), manoDe(AS, DOS, AS, DOS)};
    VERIFICAR(probabilidadesVictoria2Fija(seguras, 0, GRANDE, NADA, NADA) ==
              1.0);
    destruirMano(&seguras[0]);
    destruirMano(&seguras[1]);

    // Con todos los pitos en la pareja, cualquier rival tiene una mano
    // estrictamente mayor a la grande
    Mano perdedoras[2] = {manoDe(AS, AS, AS, AS), manoDe(DOS, DOS, DOS, DOS)};
    VERIFICAR(probabilidadesVictoria2Fija(perdedoras, 0, GRANDE, NADA, NADA) ==
              0.0);
    destruirMano(&perdedoras[0]);
    destruirMano(&perdedoras[1]);
}

static void testProbabilidadesRangoYMano(void) {
    Mano manos[2] = {manoDe(TRES, REY, SOTA, SEIS),
                     manoDe(REY, SIETE, CUATRO, AS)};

    double pMano0 = probabilidadesVictoria2Fija(manos, 0, GRANDE, NADA, NADA);
    VERIFICAR(pMano0 > 0.0 && pMano0 < 1.0);
    // Valor exacto contrastado con un Monte Carlo independiente
    VERIFICAR(fabs(pMano0 - 0.923961191447) < 1e-9);

    // Ser mano nunca puede empeorar la probabilidad de la pareja
    double pMano1 = probabilidadesVictoria2Fija(manos, 1, GRANDE, NADA, NADA);
    VERIFICAR(pMano1 > 0.0 && pMano1 < 1.0);
    VERIFICAR(pMano0 >= pMano1);

    destruirMano(&manos[0]);
    destruirMano(&manos[1]);
}

static void testProbabilidadesEntradasInvalidas(void) {
    VERIFICAR(probabilidadesVictoria2Fija(NULL, 0, GRANDE, NADA, NADA) ==
              -1.0);

    Mano sinCartas[2] = {{0}, {0}};
    VERIFICAR(probabilidadesVictoria2Fija(sinCartas, 0, GRANDE, NADA, NADA) ==
              -1.0);
}

static void testImprimirEntradasInvalidas(void) {
    Mano sinCartas = {0};
    VERIFICAR(imprimirMano(sinCartas) == 1);
    VERIFICAR(imprimirManos(NULL) == 1);
    VERIFICAR(imprimirNumeroRonda(0) == 1);
    VERIFICAR(imprimirGanadorLance(NULL, 0) == 1);
    VERIFICAR(imprimirGanadorLance("Grande", -1) == 1);
    VERIFICAR(imprimirGanadorLance("Grande", NUMERO_JUGADORES_MUS) == 1);
    VERIFICAR(imprimirTantos(NULL) == 1);
    VERIFICAR(imprimirGanadorPartida(NULL) == 1);

    Carta invalida = {.numero = 0, .palo = 0};
    VERIFICAR(imprimirCarta(invalida) == 1);

    // Una partida sin ganador tampoco se puede imprimir
    PartidaMus sinGanador = {0};
    VERIFICAR(imprimirGanadorPartida(&sinGanador) == 1);
}

int main(void) {
    srand(42);
    testSimularRondaMus();
    testPartidaPorRondas();
    testSimularPartidaMus();
    testProbabilidadesCasosSeguros();
    testProbabilidadesRangoYMano();
    testProbabilidadesEntradasInvalidas();
    testImprimirEntradasInvalidas();
    return resumenPruebas("test_mus_sim");
}
