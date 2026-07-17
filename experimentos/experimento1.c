#include "baraja_espanola.h"
#include "mus_sim.h"
#include <stdio.h>

int main() {
    Mano jugador1 = {0};
    if (crearManoMus(&jugador1) == 1)
        return 1;
    jugador1.cartas[0].numero = TRES;
    jugador1.cartas[1].numero = REY;
    jugador1.cartas[2].numero = SOTA;
    jugador1.cartas[3].numero = SEIS;
    Mano jugador3 = {0};
    if (crearManoMus(&jugador3) == 1)
        return 1;
    jugador3.cartas[0].numero = REY;
    jugador3.cartas[1].numero = SIETE;
    jugador3.cartas[2].numero = CUATRO;
    jugador3.cartas[3].numero = AS;

    Mano manos[2] = {jugador1, jugador3};

    double exito = probabilidadesVictoria2Fija(manos, 0, GRANDE, NADA, NADA);
    printf("%.4f  \n", exito);
    destruirMano(&jugador1);
    destruirMano(&jugador3);
}
