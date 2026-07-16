#include "baraja_espanola.h"
#include "utiles_test.h"

// Cuenta cuántas veces aparece cada combinación (numero, palo)
static void contarCartas(const Baraja *baraja, int conteo[4][13]) {
    for (int p = 0; p < 4; p++)
        for (int n = 0; n < 13; n++)
            conteo[p][n] = 0;
    for (size_t i = 0; i < baraja->tamano; i++) {
        Carta carta = baraja->cartas[i];
        VERIFICAR(carta.palo >= OROS && carta.palo <= BASTOS);
        VERIFICAR(carta.numero >= AS && carta.numero <= REY);
        conteo[carta.palo][carta.numero]++;
    }
}

static void testBaraja40(void) {
    VERIFICAR(crearBarajaEspanola40(NULL) == 1);

    Baraja baraja;
    VERIFICAR(crearBarajaEspanola40(&baraja) == 0);
    VERIFICAR(baraja.tamano == 40);

    int conteo[4][13];
    contarCartas(&baraja, conteo);
    for (int p = OROS; p <= BASTOS; p++) {
        for (int n = AS; n <= REY; n++) {
            if (n == OCHO || n == NUEVE)
                VERIFICAR(conteo[p][n] == 0); // Sin ochos ni nueves
            else
                VERIFICAR(conteo[p][n] == 1); // Cada carta exactamente una vez
        }
    }

    destruirBaraja(&baraja);
}

static void testBaraja48(void) {
    VERIFICAR(crearBarajaEspanola48(NULL) == 1);

    Baraja baraja;
    VERIFICAR(crearBarajaEspanola48(&baraja) == 0);
    VERIFICAR(baraja.tamano == 48);

    int conteo[4][13];
    contarCartas(&baraja, conteo);
    for (int p = OROS; p <= BASTOS; p++)
        for (int n = AS; n <= REY; n++)
            VERIFICAR(conteo[p][n] == 1);

    destruirBaraja(&baraja);
}

int main(void) {
    testBaraja40();
    testBaraja48();
    return resumenPruebas("test_baraja_espanola");
}
