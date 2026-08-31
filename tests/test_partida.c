#include <stdlib.h>

#include "baraja_espanola.h"
#include "mus.h"
#include "utiles_test.h"

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

// Devuelve cuántas cartas repetidas hay entre las 4 manos, verificando de
// paso que todas son cartas válidas de la baraja de 40
static int duplicadosEnManos(PartidaMus *partida) {
    int conteo[4][13] = {{0}};
    int duplicados = 0;
    for (int m = 0; m < NUMERO_JUGADORES_MUS; m++)
        for (size_t i = 0; i < partida->manos[m].tamano; i++) {
            Carta carta = partida->manos[m].cartas[i];
            VERIFICAR(carta.palo >= OROS && carta.palo <= BASTOS);
            VERIFICAR(carta.numero >= AS && carta.numero <= REY);
            VERIFICAR(carta.numero != OCHO && carta.numero != NUEVE);
            if (++conteo[carta.palo][carta.numero] > 1)
                duplicados++;
        }
    return duplicados;
}

static void testIniciarDestruirPartida(void) {
    VERIFICAR(reiniciarEnvitesRonda(NULL) == 1);
    VERIFICAR(iniciarPartidaMus(NULL) == 1);
    VERIFICAR(destruirPartidaMus(NULL) == 1);

    PartidaMus partida;
    VERIFICAR(iniciarPartidaMus(&partida) == 0);
    VERIFICAR(partida.baraja.cartas != NULL);
    VERIFICAR(partida.baraja.tamano == 40);
    VERIFICAR(partida.baraja.siguiente_carta == 0);
    VERIFICAR(partida.descartes.cartas != NULL);
    VERIFICAR(partida.descartes.siguiente_carta == 0); // Monton vacio
    for (int m = 0; m < NUMERO_JUGADORES_MUS; m++) {
        VERIFICAR(partida.manos[m].cartas != NULL);
        VERIFICAR(partida.manos[m].tamano == TAMANO_MANO_MUS);
    }
    VERIFICAR(partida.tantos[0] == 0);
    VERIFICAR(partida.tantos[1] == 0);
    VERIFICAR(partida.mano == 0);
    VERIFICAR(partida.envites_actuales.grande == 0);
    VERIFICAR(partida.envites_actuales.chica == 0);
    VERIFICAR(partida.envites_actuales.pares == 0);
    VERIFICAR(partida.envites_actuales.juego == 0);
    VERIFICAR(partida.envites_actuales.punto == 0);

    // ASan/LSan verifica que no queda nada sin liberar
    VERIFICAR(destruirPartidaMus(&partida) == 0);
    VERIFICAR(partida.baraja.cartas == NULL);
    VERIFICAR(partida.descartes.cartas == NULL);
    for (int m = 0; m < NUMERO_JUGADORES_MUS; m++)
        VERIFICAR(partida.manos[m].cartas == NULL);
}

static void testRepartirManos(void) {
    VERIFICAR(repartirManos(NULL) == 1);

    PartidaMus partida;
    iniciarPartidaMus(&partida);
    barajar(&partida.baraja);

    VERIFICAR(repartirManos(&partida) == 0);
    VERIFICAR(partida.baraja.siguiente_carta == 16); // 4 jugadores x 4 cartas
    VERIFICAR(duplicadosEnManos(&partida) == 0);

    destruirPartidaMus(&partida);
}

static void testManoSeDescarta(void) {
    PartidaMus partida;
    iniciarPartidaMus(&partida);
    barajar(&partida.baraja);
    repartirManos(&partida);

    int descartadas[TAMANO_MANO_MUS] = {1, 0, 1, 0};
    VERIFICAR(manoSeDescarta(NULL, &partida.manos[0], descartadas) == 1);
    VERIFICAR(manoSeDescarta(&partida, NULL, descartadas) == 1);

    Carta antes0 = partida.manos[0].cartas[0];
    Carta antes1 = partida.manos[0].cartas[1];
    Carta antes2 = partida.manos[0].cartas[2];
    VERIFICAR(manoSeDescarta(&partida, &partida.manos[0], descartadas) == 0);

    // Las dos descartadas acaban en el monton, en orden
    VERIFICAR(partida.descartes.siguiente_carta == 2);
    VERIFICAR(partida.descartes.cartas[0].numero == antes0.numero);
    VERIFICAR(partida.descartes.cartas[0].palo == antes0.palo);
    VERIFICAR(partida.descartes.cartas[1].numero == antes2.numero);
    VERIFICAR(partida.descartes.cartas[1].palo == antes2.palo);

    // Las no descartadas se conservan y se roban 2 cartas nuevas
    VERIFICAR(partida.manos[0].cartas[1].numero == antes1.numero);
    VERIFICAR(partida.manos[0].cartas[1].palo == antes1.palo);
    VERIFICAR(partida.baraja.siguiente_carta == 18);
    VERIFICAR(duplicadosEnManos(&partida) == 0);

    destruirPartidaMus(&partida);
}

static void testRecicladoSinDuplicados(void) {
    PartidaMus partida;
    iniciarPartidaMus(&partida);
    barajar(&partida.baraja);
    repartirManos(&partida);

    // Todos descartan sus 4 cartas cada ronda: 16 robos por ronda fuerzan
    // reciclar el monton de descartes varias veces
    int todas[TAMANO_MANO_MUS] = {1, 1, 1, 1};
    for (int ronda = 0; ronda < 20; ronda++) {
        for (int m = 0; m < NUMERO_JUGADORES_MUS; m++)
            VERIFICAR(manoSeDescarta(&partida, &partida.manos[m], todas) == 0);
        VERIFICAR(duplicadosEnManos(&partida) == 0);
        // Tras reciclar, la baraja nunca puede tener mas de 24 cartas
        // (40 menos las 16 que estan en las manos)
        VERIFICAR(partida.baraja.tamano <= 40);
        VERIFICAR(partida.baraja.siguiente_carta <= partida.baraja.tamano);
    }

    destruirPartidaMus(&partida);
}

static void testPuntuarPares(void) {
    VERIFICAR(puntuarPares(NULL) == -1);

    PartidaMus partida;
    iniciarPartidaMus(&partida);
    for (int i = 0; i < NUMERO_JUGADORES_MUS; i++)
        destruirMano(&partida.manos[i]);
    partida.manos[0] = manoDe(REY, REY, AS, AS);       // Dúplex: 3
    partida.manos[1] = manoDe(CABALLO, CABALLO, AS, 4);// Par: 1
    partida.manos[2] = manoDe(SOTA, SOTA, SOTA, 4);    // Medias: 2
    partida.manos[3] = manoDe(SIETE, SIETE, AS, 4);    // Par: 1
    partida.envites_actuales.pares = 4;
    VERIFICAR(puntuarPares(&partida) == 0);
    VERIFICAR(partida.tantos[0] == 9); // Envite 4 + dúplex 3 + medias 2
    VERIFICAR(partida.tantos[1] == 0);

    partida.envites_actuales.pares = 0;
    partida.manos[0].cartas[1].numero = CABALLO;
    partida.manos[0].cartas[3].numero = CUATRO;
    partida.manos[2].cartas[1].numero = CABALLO;
    partida.manos[2].cartas[2].numero = SIETE;
    VERIFICAR(parejaTienePares(partida.manos, 0) == 0);
    VERIFICAR(puntuarPares(&partida) == 0);
    VERIFICAR(partida.tantos[0] == 9);
    VERIFICAR(partida.tantos[1] == 2);
    partida.envites_actuales.pares = 2;
    VERIFICAR(puntuarPares(&partida) == -1);
    VERIFICAR(partida.tantos[1] == 2);
    destruirPartidaMus(&partida);
}

int main(void) {
    srand(88);
    testIniciarDestruirPartida();
    testRepartirManos();
    testManoSeDescarta();
    testRecicladoSinDuplicados();
    testPuntuarPares();
    return resumenPruebas("test_partida");
}
