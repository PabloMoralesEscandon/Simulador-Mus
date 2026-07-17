#include "baraja_espanola.h"
#include "mus.h"
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

static void liberarManos(Mano manos[NUMERO_JUGADORES_MUS]) {
    for (int i = 0; i < NUMERO_JUGADORES_MUS; i++)
        destruirMano(&manos[i]);
}

static void testCrearManoMus(void) {
    Mano mano;
    VERIFICAR(crearManoMus(&mano) == 0);
    VERIFICAR(mano.tamano == TAMANO_MANO_MUS);
    destruirMano(&mano);
}

static void testGrande(void) {
    // Cuatro reyes ganan a cualquier otra mano
    Mano manos[NUMERO_JUGADORES_MUS] = {
        manoDe(CABALLO, CABALLO, CABALLO, CABALLO),
        manoDe(REY, REY, REY, REY),
        manoDe(SOTA, SOTA, SOTA, SOTA),
        manoDe(REY, REY, REY, CABALLO),
    };
    VERIFICAR(ganadorGrande(manos, 0) == 1);
    liberarManos(manos);

    // La carta más alta manda: rey-caballo-as-as gana a cuatro caballos
    Mano manos2[NUMERO_JUGADORES_MUS] = {
        manoDe(CABALLO, CABALLO, CABALLO, CABALLO),
        manoDe(REY, CABALLO, AS, AS),
        manoDe(AS, AS, AS, AS),
        manoDe(SOTA, SOTA, SOTA, SOTA),
    };
    VERIFICAR(ganadorGrande(manos2, 0) == 1);
    liberarManos(manos2);

    // El tres cuenta como rey: en empate gana el más cercano a la mano
    Mano manos3[NUMERO_JUGADORES_MUS] = {
        manoDe(REY, REY, REY, REY),
        manoDe(TRES, TRES, TRES, TRES),
        manoDe(SOTA, SOTA, SOTA, SOTA),
        manoDe(CABALLO, CABALLO, CABALLO, CABALLO),
    };
    VERIFICAR(ganadorGrande(manos3, 0) == 0);
    VERIFICAR(ganadorGrande(manos3, 1) == 1);
    VERIFICAR(ganadorGrande(manos3, 2) == 0); // Desde el 2, el 0 va antes que el 1
    liberarManos(manos3);
}

static void testChica(void) {
    // Los pitos (ases y doses) son las cartas más bajas
    Mano manos[NUMERO_JUGADORES_MUS] = {
        manoDe(CUATRO, CUATRO, CINCO, SEIS),
        manoDe(AS, DOS, CUATRO, REY),
        manoDe(SOTA, SOTA, SOTA, SOTA),
        manoDe(CINCO, CINCO, SEIS, SIETE),
    };
    VERIFICAR(ganadorChica(manos, 0) == 1);
    liberarManos(manos);

    // El dos cuenta como as: en empate gana el más cercano a la mano
    Mano manos2[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, AS, AS, AS),
        manoDe(DOS, DOS, DOS, DOS),
        manoDe(REY, REY, REY, REY),
        manoDe(SOTA, SOTA, SOTA, SOTA),
    };
    VERIFICAR(ganadorChica(manos2, 0) == 0);
    VERIFICAR(ganadorChica(manos2, 1) == 1);
    liberarManos(manos2);
}

static void testPar(void) {
    // Duplex gana a medias, medias gana a par, par gana a no par
    Mano manos[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, CUATRO, SIETE, SOTA),   // Sin pares
        manoDe(REY, REY, CINCO, SEIS),     // Par de reyes
        manoDe(CABALLO, CABALLO, AS, AS),  // Duplex de caballos y ases
        manoDe(REY, REY, REY, CUATRO),     // Medias de reyes
    };
    VERIFICAR(ganadorPar(manos, 0) == 2);
    liberarManos(manos);

    // Entre pares gana el más alto, y el tres cuenta como rey
    Mano manos2[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, AS, CINCO, SEIS),       // Par de ases
        manoDe(REY, TRES, CINCO, SEIS),    // Par de reyes (rey + tres)
        manoDe(CABALLO, CABALLO, AS, SEIS),// Par de caballos
        manoDe(AS, CUATRO, SIETE, SOTA),   // Sin pares
    };
    VERIFICAR(ganadorPar(manos2, 0) == 1);
    liberarManos(manos2);

    // Entre duplex manda el par alto y luego el bajo: reyes-reyes (cuatro
    // reyes, contando treses) > reyes-ases > caballos-sotas
    Mano manos3[NUMERO_JUGADORES_MUS] = {
        manoDe(CABALLO, CABALLO, SOTA, SOTA),
        manoDe(REY, REY, AS, AS),
        manoDe(REY, REY, TRES, TRES),
        manoDe(AS, CUATRO, SIETE, SOTA),
    };
    VERIFICAR(ganadorPar(manos3, 0) == 2);
    liberarManos(manos3);

    // Empate de duplex: gana el más cercano a la mano
    Mano manos5[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, CUATRO, SIETE, SOTA),
        manoDe(REY, REY, AS, AS),
        manoDe(DOS, CINCO, SEIS, CABALLO),
        manoDe(REY, TRES, AS, DOS),
    };
    VERIFICAR(ganadorPar(manos5, 0) == 1);
    VERIFICAR(ganadorPar(manos5, 2) == 3);
    liberarManos(manos5);

    // Si nadie tiene pares, gana la mano
    Mano manos4[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, CUATRO, SIETE, SOTA),
        manoDe(DOS, CINCO, SEIS, CABALLO),
        manoDe(AS, CINCO, SIETE, CABALLO),
        manoDe(DOS, CUATRO, SEIS, SOTA),
    };
    VERIFICAR(ganadorPar(manos4, 3) == 3);
    liberarManos(manos4);
}

static void testPunto(void) {
    // 31 es la mejor jugada
    Mano manos[NUMERO_JUGADORES_MUS] = {
        manoDe(REY, REY, SEIS, SEIS),      // 32
        manoDe(REY, REY, REY, AS),         // 31
        manoDe(REY, REY, SOTA, CABALLO),   // 40
        manoDe(REY, REY, REY, SIETE),      // 37
    };
    VERIFICAR(ganadorJuego(manos, 0) == 1);
    liberarManos(manos);

    // Tras el 31 y el 32, manda el 40 y luego de 37 hacia abajo
    Mano manos2[NUMERO_JUGADORES_MUS] = {
        manoDe(REY, REY, SEIS, SIETE),     // 33
        manoDe(REY, REY, REY, SIETE),      // 37
        manoDe(REY, REY, SOTA, CABALLO),   // 40
        manoDe(REY, REY, SEIS, SEIS),      // 32
    };
    VERIFICAR(ganadorJuego(manos2, 0) == 3);
    liberarManos(manos2);

    // En empate a 31 gana el más cercano a la mano
    Mano manos3[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, AS, AS, AS),            // 4, sin juego
        manoDe(REY, REY, REY, AS),         // 31
        manoDe(AS, AS, AS, AS),            // 4, sin juego
        manoDe(REY, REY, CABALLO, DOS),    // 31
    };
    VERIFICAR(ganadorJuego(manos3, 0) == 1);
    VERIFICAR(ganadorJuego(manos3, 2) == 3);
    liberarManos(manos3);
}

static void testGanadorPunto(void) {
    // Gana la suma más alta
    Mano manos[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, AS, AS, AS),             // 4
        manoDe(REY, SOTA, CINCO, CUATRO),   // 29
        manoDe(REY, SOTA, SEIS, CUATRO),    // 30
        manoDe(CABALLO, SOTA, SIETE, AS),   // 28
    };
    VERIFICAR(ganadorPunto(manos, 0) == 2);
    liberarManos(manos);

    // En empate gana el más cercano a la mano
    Mano manos2[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, AS, AS, AS),               // 4
        manoDe(REY, SOTA, SEIS, CUATRO),      // 30
        manoDe(DOS, DOS, DOS, DOS),           // 4
        manoDe(SOTA, CABALLO, SEIS, CUATRO),  // 30
    };
    VERIFICAR(ganadorPunto(manos2, 0) == 1);
    VERIFICAR(ganadorPunto(manos2, 2) == 3);
    VERIFICAR(ganadorPunto(manos2, 3) == 3);
    liberarManos(manos2);

    // Si nadie tiene juego, el lance de juego se decide al punto
    Mano manos3[NUMERO_JUGADORES_MUS] = {
        manoDe(AS, AS, AS, AS),            // 4
        manoDe(REY, SOTA, SEIS, CUATRO),   // 30
        manoDe(SIETE, SEIS, CINCO, AS),    // 19
        manoDe(DOS, DOS, DOS, DOS),        // 4
    };
    VERIFICAR(ganadorJuego(manos3, 0) == 1);
    VERIFICAR(ganadorJuego(manos3, 2) == 1);
    liberarManos(manos3);

    // Cualquier juego (aunque sea 33, el peor) gana a las manos sin juego
    Mano manos4[NUMERO_JUGADORES_MUS] = {
        manoDe(REY, SOTA, SEIS, CUATRO),   // 30, sin juego
        manoDe(AS, AS, AS, AS),            // 4, sin juego
        manoDe(REY, REY, SEIS, SIETE),     // 33
        manoDe(DOS, DOS, DOS, DOS),        // 4, sin juego
    };
    VERIFICAR(ganadorJuego(manos4, 0) == 2);
    liberarManos(manos4);
}

int main(void) {
    testCrearManoMus();
    testGrande();
    testChica();
    testPar();
    testPunto();
    testGanadorPunto();
    return resumenPruebas("test_mus");
}
