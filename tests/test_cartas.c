#include <stdlib.h>

#include "cartas.h"
#include "utiles_test.h"

static void testCrearDestruirBaraja(void) {
    VERIFICAR(crearBaraja(NULL, 40) == 1);

    Baraja baraja;
    VERIFICAR(crearBaraja(&baraja, 40) == 0);
    VERIFICAR(baraja.cartas != NULL);
    VERIFICAR(baraja.tamano == 40);
    VERIFICAR(baraja.siguiente_carta == 0);

    VERIFICAR(destruirBaraja(&baraja) == 0);
    VERIFICAR(baraja.cartas == NULL);
    VERIFICAR(baraja.tamano == 0);

    VERIFICAR(destruirBaraja(NULL) == 1);
}

static void testBarajarConservaCartas(void) {
    Baraja baraja;
    crearBaraja(&baraja, 40);
    for (size_t i = 0; i < baraja.tamano; i++) {
        baraja.cartas[i].numero = (int)i;
        baraja.cartas[i].palo = 0;
    }

    VERIFICAR(barajar(&baraja) == 0);

    // Tras barajar deben seguir estando las mismas 40 cartas
    int visto[40] = {0};
    for (size_t i = 0; i < baraja.tamano; i++) {
        int n = baraja.cartas[i].numero;
        VERIFICAR(n >= 0 && n < 40);
        visto[n]++;
    }
    for (int i = 0; i < 40; i++)
        VERIFICAR(visto[i] == 1);

    destruirBaraja(&baraja);
}

static void testBarajarCasosInvalidos(void) {
    VERIFICAR(barajar(NULL) == 1);

    Baraja vacia = {NULL, 0, 0};
    VERIFICAR(barajar(&vacia) == 1);
}

static void testSacarCarta(void) {
    Baraja baraja;
    crearBaraja(&baraja, 3);
    for (size_t i = 0; i < baraja.tamano; i++) {
        baraja.cartas[i].numero = (int)i + 1;
        baraja.cartas[i].palo = (int)i;
    }

    Carta carta;
    VERIFICAR(sacarCarta(&baraja, &carta) == 0);
    VERIFICAR(carta.numero == 1 && carta.palo == 0);
    VERIFICAR(sacarCarta(&baraja, &carta) == 0);
    VERIFICAR(carta.numero == 2 && carta.palo == 1);
    VERIFICAR(sacarCarta(&baraja, &carta) == 0);
    VERIFICAR(carta.numero == 3 && carta.palo == 2);

    // Baraja agotada
    VERIFICAR(sacarCarta(&baraja, &carta) == 1);

    VERIFICAR(sacarCarta(NULL, &carta) == 1);
    VERIFICAR(sacarCarta(&baraja, NULL) == 1);

    destruirBaraja(&baraja);
}

static void testCrearDestruirMano(void) {
    VERIFICAR(crearMano(NULL, 4) == 1);

    Mano mano;
    VERIFICAR(crearMano(&mano, 4) == 0);
    VERIFICAR(mano.cartas != NULL);
    VERIFICAR(mano.tamano == 4);

    VERIFICAR(destruirMano(&mano) == 0);
    VERIFICAR(mano.cartas == NULL);
    VERIFICAR(mano.tamano == 0);

    VERIFICAR(destruirMano(NULL) == 1);
}

static void testOrdenarMano(void) {
    VERIFICAR(ordenarMano(NULL) == 1);

    Mano vacia = {NULL, 0};
    VERIFICAR(ordenarMano(&vacia) == 1);

    Mano mano;
    crearMano(&mano, 4);
    int numeros[4] = {7, 1, 12, 4};
    for (size_t i = 0; i < mano.tamano; i++) {
        mano.cartas[i].numero = numeros[i];
        mano.cartas[i].palo = 0;
    }

    VERIFICAR(ordenarMano(&mano) == 0);
    VERIFICAR(mano.cartas[0].numero == 1);
    VERIFICAR(mano.cartas[1].numero == 4);
    VERIFICAR(mano.cartas[2].numero == 7);
    VERIFICAR(mano.cartas[3].numero == 12);

    destruirMano(&mano);
}

int main(void) {
    srand(88);
    testCrearDestruirBaraja();
    testBarajarConservaCartas();
    testBarajarCasosInvalidos();
    testSacarCarta();
    testCrearDestruirMano();
    testOrdenarMano();
    return resumenPruebas("test_cartas");
}
