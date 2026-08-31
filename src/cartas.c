#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "cartas.h"

/** Devuelve un entero uniforme en [0, limite). */
static size_t aleatorioAcotado(size_t limite) {
    const uintmax_t rangoRand = (uintmax_t)RAND_MAX + 1;

    if ((uintmax_t)limite <= rangoRand) {
        const uintmax_t zonaUniforme = rangoRand - rangoRand % limite;
        unsigned int valor;
        do {
            valor = (unsigned int)rand();
        } while ((uintmax_t)valor >= zonaUniforme);
        return (size_t)((uintmax_t)valor % limite);
    }

    // Para barajas mayores que el rango de rand(), compone los bits
    // necesarios. Si RAND_MAX + 1 es impar, descarta su último resultado para
    // que cada bit siga teniendo exactamente la misma probabilidad.
    size_t mascara = limite - 1;
    for (size_t desplazamiento = 1;
         desplazamiento < sizeof(size_t) * CHAR_BIT;
         desplazamiento <<= 1)
        mascara |= mascara >> desplazamiento;

    size_t valor;
    do {
        valor = 0;
        for (size_t bit = mascara; bit != 0; bit >>= 1) {
            unsigned int muestra;
            do {
                muestra = (unsigned int)rand();
            } while (RAND_MAX % 2 == 0 && muestra == (unsigned int)RAND_MAX);
            valor = (valor << 1) | (muestra & 1U);
        }
    } while (valor >= limite);
    return valor;
}

int crearBaraja(Baraja *baraja, size_t tamano) {
    // Caso de puntero NULL
    if (baraja == NULL || tamano == 0 || tamano > SIZE_MAX / sizeof(Carta))
        return 1;
    // Alocar memoria para las cartas
    baraja->cartas = malloc(tamano * sizeof(Carta));
    if (baraja->cartas == NULL)
        return 1;
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

    // Fisher-Yates: recorre desde el final intercambiando cada posición
    // con un índice aleatorio entre 0 e i
    for (size_t i = baraja->tamano - 1; i > 0; i--) {
        size_t j = aleatorioAcotado(i + 1);

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
    if (mano == NULL || tamano == 0 || tamano > SIZE_MAX / sizeof(Carta))
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
