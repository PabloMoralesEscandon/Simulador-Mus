#include "baraja_espanola.h"

int crearBarajaEspanola40(Baraja *baraja) {
    if (baraja == NULL)
        return 1;
    if (crearBaraja(baraja, 40) == 1)
        return 1;
    int numeros[] = {AS,   DOS,   TRES, CUATRO,  CINCO,
                     SEIS, SIETE, SOTA, CABALLO, REY};
    int palos[] = {OROS, COPAS, ESPADAS, BASTOS};
    size_t num_palos = sizeof(palos) / sizeof(palos[0]);
    size_t num_numeros = sizeof(numeros) / sizeof(numeros[0]);
    for (size_t i = 0; i < num_palos; i++)
        for (size_t j = 0; j < num_numeros; j++) {
            baraja->cartas[i * 10 + j].numero = numeros[j];
            baraja->cartas[i * 10 + j].palo = palos[i];
        }
    return 0;
}

int crearBarajaEspanola48(Baraja *baraja) {
    if (baraja == NULL)
        return 1;
    if (crearBaraja(baraja, 48) == 1)
        return 1;
    int numeros[] = {AS,    DOS,  TRES,  CUATRO, CINCO,   SEIS,
                     SIETE, OCHO, NUEVE, SOTA,   CABALLO, REY};
    int palos[] = {OROS, COPAS, ESPADAS, BASTOS};
    size_t num_palos = sizeof(palos) / sizeof(palos[0]);
    size_t num_numeros = sizeof(numeros) / sizeof(numeros[0]);
    for (size_t i = 0; i < num_palos; i++)
        for (size_t j = 0; j < num_numeros; j++) {
            baraja->cartas[i * 12 + j].numero = numeros[j];
            baraja->cartas[i * 12 + j].palo = palos[i];
        }
    return 0;
}
