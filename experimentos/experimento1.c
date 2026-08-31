#include "baraja_espanola.h"
#include "mus_sim.h"
#include <stdio.h>

int main(void) {
    Mano jugador1 = {0};
    if (crearManoMus(&jugador1))
        return 1;
    jugador1.cartas[0] = (Carta){.numero = TRES, .palo = OROS};
    jugador1.cartas[1] = (Carta){.numero = REY, .palo = COPAS};
    jugador1.cartas[2] = (Carta){.numero = SOTA, .palo = ESPADAS};
    jugador1.cartas[3] = (Carta){.numero = SEIS, .palo = BASTOS};
    Mano jugador3 = {0};
    if (crearManoMus(&jugador3)) {
        destruirMano(&jugador1);
        return 1;
    }
    jugador3.cartas[0] = (Carta){.numero = REY, .palo = ESPADAS};
    jugador3.cartas[1] = (Carta){.numero = SIETE, .palo = OROS};
    jugador3.cartas[2] = (Carta){.numero = CUATRO, .palo = COPAS};
    jugador3.cartas[3] = (Carta){.numero = AS, .palo = BASTOS};

    Mano manos[2] = {jugador1, jugador3};

    double exito = probabilidadesVictoria2Fija(manos, 0, GRANDE, NADA, NADA);
    if (exito < 0.0) {
        destruirMano(&jugador1);
        destruirMano(&jugador3);
        return 1;
    }
    printf("%.4f\n", exito);
    destruirMano(&jugador1);
    destruirMano(&jugador3);
    return 0;
}
