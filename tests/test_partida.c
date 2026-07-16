#include <stdlib.h>

#include "baraja_espanola.h"
#include "mus.h"
#include "utiles_test.h"

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

int main(void) {
    srand(88);
    testIniciarDestruirPartida();
    testRepartirManos();
    testManoSeDescarta();
    testRecicladoSinDuplicados();
    return resumenPruebas("test_partida");
}
