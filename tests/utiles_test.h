#ifndef UTILES_TEST_H
#define UTILES_TEST_H

#include <stdio.h>

static int pruebas_totales = 0;
static int pruebas_fallidas = 0;

#define VERIFICAR(cond)                                                        \
    do {                                                                       \
        pruebas_totales++;                                                     \
        if (!(cond)) {                                                         \
            pruebas_fallidas++;                                                \
            printf("FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond);            \
        }                                                                      \
    } while (0)

static int resumenPruebas(const char *nombre) {
    if (pruebas_fallidas == 0) {
        printf("%s: %d pruebas OK\n", nombre, pruebas_totales);
        return 0;
    }
    printf("%s: %d de %d pruebas fallidas\n", nombre, pruebas_fallidas,
           pruebas_totales);
    return 1;
}

#endif
