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
    VERIFICAR(iniciarEnviteMus(NULL) == 1);
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

static void testIniciarEnviteMus(void) {
    EnviteMus envite;
    VERIFICAR(iniciarEnviteMus(&envite) == 0);
    VERIFICAR(envite.estado == ENVITE_AL_PASO);
    VERIFICAR(envite.cantidad == 0);
    VERIFICAR(envite.cantidadAnterior == 0);
    VERIFICAR(envite.parejaApostadora == -1);
}

static void testEnvidarMus(void) {
    VERIFICAR(envidarMus(NULL, 0, 2) == 1);
    EnviteMus envite;
    iniciarEnviteMus(&envite);
    VERIFICAR(envidarMus(&envite, -1, 2) == 1);
    VERIFICAR(envidarMus(&envite, 0, 1) == 1);
    VERIFICAR(envidarMus(&envite, 0, 2) == 0);
    VERIFICAR(envite.estado == ENVITE_PENDIENTE);
    VERIFICAR(envite.cantidad == 2);
    VERIFICAR(envite.cantidadAnterior == 1);
    VERIFICAR(envite.parejaApostadora == 0);
    VERIFICAR(envidarMus(&envite, 0, 4) == 1);
    VERIFICAR(envidarMus(&envite, 1, 2) == 1);
    VERIFICAR(envidarMus(&envite, 1, 6) == 0);
    VERIFICAR(envite.cantidad == 6);
    VERIFICAR(envite.cantidadAnterior == 2);
    VERIFICAR(envite.parejaApostadora == 1);
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

static void testTodosDanMus(void) {
    int todos[NUMERO_JUGADORES_MUS] = {1, 1, 1, 1};
    int cortan[NUMERO_JUGADORES_MUS] = {1, 1, 0, 1};
    int invalidas[NUMERO_JUGADORES_MUS] = {1, 1, 2, 1};
    VERIFICAR(todosDanMus(NULL) == -1);
    VERIFICAR(todosDanMus(todos) == 1);
    VERIFICAR(todosDanMus(cortan) == 0);
    VERIFICAR(todosDanMus(invalidas) == -1);
}

static void testDescartarManosMus(void) {
    VERIFICAR(descartarManosMus(NULL, NULL) == 1);

    PartidaMus partida;
    iniciarPartidaMus(&partida);
    barajar(&partida.baraja);
    repartirManos(&partida);
    partida.mano = 2;
    int descartadas[NUMERO_JUGADORES_MUS][TAMANO_MANO_MUS] = {
        {1, 0, 0, 0}, {1, 1, 0, 0}, {1, 1, 1, 0}, {1, 1, 1, 1}};
    VERIFICAR(descartarManosMus(&partida, descartadas) == 0);
    VERIFICAR(partida.baraja.siguiente_carta == 26);
    VERIFICAR(partida.descartes.siguiente_carta == 10);
    VERIFICAR(duplicadosEnManos(&partida) == 0);

    descartadas[0][0] = 2;
    size_t cursor = partida.baraja.siguiente_carta;
    VERIFICAR(descartarManosMus(&partida, descartadas) == 1);
    VERIFICAR(partida.baraja.siguiente_carta == cursor);
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

static void testPuntuarJuegoOPunto(void) {
    VERIFICAR(puntuarJuegoOPunto(NULL) == -1);

    PartidaMus partida;
    iniciarPartidaMus(&partida);
    for (int i = 0; i < NUMERO_JUGADORES_MUS; i++)
        destruirMano(&partida.manos[i]);
    partida.manos[0] = manoDe(REY, REY, REY, AS);       // 31: 3
    partida.manos[1] = manoDe(REY, REY, REY, SIETE);    // 37: 2
    partida.manos[2] = manoDe(REY, REY, SEIS, SEIS);    // 32: 2
    partida.manos[3] = manoDe(REY, REY, SOTA, CABALLO); // 40: 2
    partida.envites_actuales.juego = 4;
    VERIFICAR(puntuarJuegoOPunto(&partida) == 0);
    VERIFICAR(partida.tantos[0] == 9); // Envite 4 + 31 (3) + 32 (2)
    VERIFICAR(partida.tantos[1] == 0);

    partida.tantos[0] = 0;
    partida.envites_actuales.juego = 0;
    for (int i = 0; i < NUMERO_JUGADORES_MUS; i++) {
        partida.manos[i].cartas[0].numero = AS;
        partida.manos[i].cartas[1].numero = CUATRO;
    }
    partida.manos[0].cartas[2].numero = SIETE;
    partida.manos[0].cartas[3].numero = SIETE; // 19
    partida.manos[1].cartas[2].numero = SIETE;
    partida.manos[1].cartas[3].numero = REY;   // 22
    partida.manos[2].cartas[2].numero = SEIS;
    partida.manos[2].cartas[3].numero = REY;   // 21
    partida.manos[3].cartas[2].numero = SIETE;
    partida.manos[3].cartas[3].numero = REY;   // 22, gana por mano
    partida.envites_actuales.punto = 2;
    VERIFICAR(puntuarJuegoOPunto(&partida) == 0);
    VERIFICAR(partida.tantos[0] == 0);
    VERIFICAR(partida.tantos[1] == 3); // Envite 2 + punto 1
    destruirPartidaMus(&partida);
}

static void testPuntuarGrande(void) {
    VERIFICAR(puntuarGrande(NULL) == -1);

    PartidaMus partida;
    iniciarPartidaMus(&partida);
    for (int i = 0; i < NUMERO_JUGADORES_MUS; i++)
        destruirMano(&partida.manos[i]);
    partida.manos[0] = manoDe(REY, REY, AS, AS);
    partida.manos[1] = manoDe(CABALLO, CABALLO, CABALLO, CABALLO);
    partida.manos[2] = manoDe(SOTA, SOTA, SOTA, SOTA);
    partida.manos[3] = manoDe(REY, REY, REY, CABALLO);
    VERIFICAR(puntuarGrande(&partida) == 0);
    VERIFICAR(partida.tantos[1] == 1);

    partida.envites_actuales.grande = 4;
    VERIFICAR(puntuarGrande(&partida) == 0);
    VERIFICAR(partida.tantos[1] == 5);
    destruirPartidaMus(&partida);
}

static void testPuntuarChica(void) {
    VERIFICAR(puntuarChica(NULL) == -1);

    PartidaMus partida;
    iniciarPartidaMus(&partida);
    for (int i = 0; i < NUMERO_JUGADORES_MUS; i++)
        destruirMano(&partida.manos[i]);
    partida.manos[0] = manoDe(REY, REY, AS, AS);
    partida.manos[1] = manoDe(DOS, DOS, CUATRO, CINCO);
    partida.manos[2] = manoDe(SOTA, SOTA, SOTA, SOTA);
    partida.manos[3] = manoDe(AS, AS, AS, CABALLO);
    VERIFICAR(puntuarChica(&partida) == 0);
    VERIFICAR(partida.tantos[1] == 1);

    partida.envites_actuales.chica = 6;
    VERIFICAR(puntuarChica(&partida) == 0);
    VERIFICAR(partida.tantos[1] == 7);
    destruirPartidaMus(&partida);
}

int main(void) {
    srand(88);
    testIniciarDestruirPartida();
    testIniciarEnviteMus();
    testEnvidarMus();
    testRepartirManos();
    testManoSeDescarta();
    testTodosDanMus();
    testDescartarManosMus();
    testRecicladoSinDuplicados();
    testPuntuarPares();
    testPuntuarJuegoOPunto();
    testPuntuarGrande();
    testPuntuarChica();
    return resumenPruebas("test_partida");
}
