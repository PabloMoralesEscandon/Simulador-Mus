#include <stdlib.h>

#include "cartas.h"

int crearBaraja(Baraja *baraja, size_t tamano) {
    // Caso de puntero NULL
    if (baraja == NULL)
        return 1;
    // Alocar memoria para las cartas
    baraja->cartas = malloc(tamano * sizeof(Carta));
    if (baraja->cartas == NULL)
        return 1;
    // Asginar valores
    baraja->tamano = tamano;
    baraja->siguiente_carta = 0;
    return 0;
}

int destruirBaraja(Baraja *baraja) {
    // Caso de puntero NULL
    if (baraja == NULL)
        return 1;
    baraja->tamano = 0;
    baraja->siguiente_carta = 0;
    free(baraja->cartas);
    baraja->cartas = NULL;
    return 0;
}

int barajar(Baraja *baraja) {
    // Caso de puntero NULL
    if (baraja == NULL)
        return 1;

    if (baraja->cartas == NULL)
        return 1;

    if (baraja->tamano == 0)
        return 1;

    // Inicializar el generador de números aleatorios
    for (size_t i = baraja->tamano - 1; i > 0; i--) {
        // Elegir un índice aleatorio entre 0 e i
        int j = rand() % (i + 1);

        // Intercambiar array[i] con array[j]
        Carta temp = baraja->cartas[i];
        baraja->cartas[i] = baraja->cartas[j];
        baraja->cartas[j] = temp;
    }
    return 0;
}

int sacarCarta(Baraja *baraja, Carta *out) {
    // Caso de puntero NULL
    if (baraja == NULL)
        return 1;

    if (out == NULL)
        return 1;

    if (baraja->cartas == NULL)
        return 1;
    if (baraja->siguiente_carta >= baraja->tamano)
        return 1;
    *out = baraja->cartas[baraja->siguiente_carta];
    baraja->siguiente_carta += 1;
    return 0;
}

int crearMano(Mano *mano, size_t tamano) {
    // Caso de puntero NULL
    if (mano == NULL)
        return 1;
    mano->cartas = malloc(tamano * sizeof(Carta));
    if (mano->cartas == NULL)
        return 1;
    mano->tamano = tamano;
    return 0;
}

int destruirMano(Mano *mano) {
    // Caso de puntero NULL
    if (mano == NULL)
        return 1;
    mano->tamano = 0;
    free(mano->cartas);
    mano->cartas = NULL;
    return 0;
}

int ordenarMano(Mano *mano) {
    // Caso de puntero NULL
    if (mano == NULL)
        return 1;
    if (mano->tamano == 0)
        return 1;
    for (size_t i = 0; i < mano->tamano; i++) {
        for (size_t j = 0; j < mano->tamano - i - 1; j++) {
            if (mano->cartas[j].numero > mano->cartas[j + 1].numero) {
                // Intercambiar
                Carta temp = mano->cartas[j];
                mano->cartas[j] = mano->cartas[j + 1];
                mano->cartas[j + 1] = temp;
            }
        }
    }
    return 0;
}
