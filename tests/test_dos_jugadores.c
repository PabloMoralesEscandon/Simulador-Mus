#include <stdlib.h>
#include <string.h>

#include "baraja_espanola.h"
#include "mus_log.h"
#include "mus_sim.h"
#include "utiles_test.h"

static Mano manoDe(int n1, int n2, int n3, int n4) {
    Mano mano = {0};
    crearManoMus(&mano);
    int numeros[TAMANO_MANO_MUS] = {n1, n2, n3, n4};
    for (size_t i = 0; i < mano.tamano; i++) {
        mano.cartas[i].numero = numeros[i];
        mano.cartas[i].palo = (int)i;
    }
    return mano;
}

static void sustituirManos(PartidaMus *partida, Mano mano0, Mano mano1) {
    destruirMano(&partida->manos[0]);
    destruirMano(&partida->manos[1]);
    partida->manos[0] = mano0;
    partida->manos[1] = mano1;
}

static void testInicializacionYReparto(void) {
    VERIFICAR(iniciarPartidaMusConJugadores(NULL, MUS_DOS_JUGADORES) == 1);
    PartidaMus invalida = {0};
    VERIFICAR(iniciarPartidaMusConJugadores(&invalida, 3) == 1);

    PartidaMus partida = {0};
    VERIFICAR(iniciarPartidaMusConJugadores(&partida, MUS_DOS_JUGADORES) ==
              0);
    VERIFICAR(partida.numeroJugadores == MUS_DOS_JUGADORES);
    VERIFICAR(partida.manos[0].cartas != NULL);
    VERIFICAR(partida.manos[1].cartas != NULL);
    VERIFICAR(partida.manos[2].cartas == NULL);
    VERIFICAR(partida.manos[3].cartas == NULL);
    VERIFICAR(barajar(&partida.baraja) == 0);
    VERIFICAR(repartirManos(&partida) == 0);
    VERIFICAR(partida.baraja.siguiente_carta == 8);

    int vistas[BASTOS + 1][REY + 1] = {{0}};
    for (int jugador = 0; jugador < partida.numeroJugadores; jugador++)
        for (size_t i = 0; i < partida.manos[jugador].tamano; i++) {
            Carta carta = partida.manos[jugador].cartas[i];
            VERIFICAR(++vistas[carta.palo][carta.numero] == 1);
        }
    VERIFICAR(destruirPartidaMus(&partida) == 0);
    VERIFICAR(partida.numeroJugadores == 0);

    PartidaMus tradicional = {0};
    VERIFICAR(iniciarPartidaMus(&tradicional) == 0);
    VERIFICAR(tradicional.numeroJugadores == MUS_CUATRO_JUGADORES);
    VERIFICAR(destruirPartidaMus(&tradicional) == 0);
}

static void testGanadoresYDesempate(void) {
    Mano manos[2] = {manoDe(REY, REY, CABALLO, AS),
                     manoDe(CABALLO, CABALLO, CABALLO, CABALLO)};
    VERIFICAR(ganadorGrandeConJugadores(manos, 2, 0) == 0);
    VERIFICAR(ganadorChicaConJugadores(manos, 2, 0) == 0);
    VERIFICAR(ganadorParConJugadores(manos, 2, 0) == 1);
    VERIFICAR(ganadorJuegoConJugadores(manos, 2, 0) == 0);
    VERIFICAR(ganadorPuntoConJugadores(manos, 2, 0) == 1);
    VERIFICAR(ganadorGrandeConJugadores(manos, 3, 0) == -1);
    VERIFICAR(ganadorGrandeConJugadores(manos, 2, 2) == -1);
    destruirMano(&manos[0]);
    destruirMano(&manos[1]);

    Mano empate[2] = {manoDe(REY, REY, AS, AS),
                      manoDe(TRES, TRES, DOS, DOS)};
    VERIFICAR(ganadorGrandeConJugadores(empate, 2, 0) == 0);
    VERIFICAR(ganadorGrandeConJugadores(empate, 2, 1) == 1);
    VERIFICAR(ganadorChicaConJugadores(empate, 2, 0) == 0);
    VERIFICAR(ganadorChicaConJugadores(empate, 2, 1) == 1);
    destruirMano(&empate[0]);
    destruirMano(&empate[1]);
}

static void testTanteoIndividual(void) {
    PartidaMus partida = {0};
    VERIFICAR(iniciarPartidaMusConJugadores(&partida, 2) == 0);
    sustituirManos(&partida, manoDe(REY, REY, AS, AS),
                   manoDe(CABALLO, CABALLO, AS, CUATRO));
    VERIFICAR(equipoTienePares(partida.manos, 2, 0) == 1);
    VERIFICAR(equipoTienePares(partida.manos, 2, 1) == 1);
    partida.envites_actuales.pares = 2;
    VERIFICAR(puntuarPares(&partida) == 0);
    VERIFICAR(partida.tantos[0] == 5); /* dúplex 3 + envite 2 */
    VERIFICAR(partida.tantos[1] == 0);

    sustituirManos(&partida, manoDe(REY, REY, REY, AS),
                   manoDe(REY, REY, REY, SIETE));
    reiniciarEnvitesRonda(&partida.envites_actuales);
    partida.envites_actuales.juego = 2;
    VERIFICAR(equipoTieneJuego(partida.manos, 2, 0) == 1);
    VERIFICAR(equipoTieneJuego(partida.manos, 2, 1) == 1);
    VERIFICAR(puntuarJuegoOPunto(&partida) == 0);
    VERIFICAR(partida.tantos[0] == 10); /* 5 + 31 (3) + envite 2 */
    VERIFICAR(partida.tantos[1] == 0);
    VERIFICAR(destruirPartidaMus(&partida) == 0);
}

typedef struct {
    int decisiones;
    int descartes;
} Contexto;

static int darMusUnaVez(const Mano *mano, int jugador, int manoPartida,
                        const int tantos[2], void *contexto) {
    (void)mano;
    (void)jugador;
    (void)manoPartida;
    (void)tantos;
    Contexto *estado = contexto;
    return estado->decisiones++ == 0;
}

static int descartarPrimera(const Mano *mano, int jugador,
                            int descartadas[TAMANO_MANO_MUS],
                            void *contexto) {
    (void)mano;
    (void)jugador;
    Contexto *estado = contexto;
    descartadas[0] = 1;
    estado->descartes++;
    return 0;
}

static AccionEnviteMus pasar(const Mano *mano, int jugador, int manoPartida,
                             const int tantos[2], Ronda ronda,
                             const EnviteMus *envite, void *contexto) {
    (void)mano;
    (void)jugador;
    (void)manoPartida;
    (void)tantos;
    (void)ronda;
    (void)envite;
    (void)contexto;
    return (AccionEnviteMus){.tipo = ACCION_PASAR};
}

typedef struct {
    AccionEnviteMus accion;
    int usada;
} ContextoEnvite;

static AccionEnviteMus accionUnica(
    const Mano *mano, int jugador, int manoPartida, const int tantos[2],
    Ronda ronda, const EnviteMus *envite, void *contexto) {
    (void)mano;
    (void)jugador;
    (void)manoPartida;
    (void)tantos;
    (void)ronda;
    (void)envite;
    ContextoEnvite *estado = contexto;
    if (estado->usada++)
        return (AccionEnviteMus){.tipo = ACCION_PASAR};
    return estado->accion;
}

static void testEnviteDosJugadores(void) {
    PartidaMus partida = {0};
    VERIFICAR(iniciarPartidaMusConJugadores(&partida, 2) == 0);
    VERIFICAR(resetearMazo(&partida) == 0);
    VERIFICAR(repartirManos(&partida) == 0);
    ContextoEnvite contextos[2] = {
        {.accion = {.tipo = ACCION_ENVIDAR, .cantidadTotal = 2}},
        {.accion = {.tipo = ACCION_QUERER}}};
    EstrategiaMus estrategias[NUMERO_JUGADORES_MUS] = {{0}};
    for (int jugador = 0; jugador < 2; jugador++) {
        estrategias[jugador].decidirEnvite = accionUnica;
        estrategias[jugador].contexto = &contextos[jugador];
    }
    EnviteMus envite = {0};
    VERIFICAR(jugarLanceEnvite(&partida, GRANDE, estrategias, &envite) == 0);
    VERIFICAR(envite.estado == ENVITE_ACEPTADO);
    VERIFICAR(partida.envites_actuales.grande == 2);
    VERIFICAR(contextos[0].usada == 1);
    VERIFICAR(contextos[1].usada == 1);
    VERIFICAR(destruirPartidaMus(&partida) == 0);
}

static void testMusSimulacionYRotacion(void) {
    PartidaMus partida = {0};
    VERIFICAR(iniciarPartidaMusConJugadores(&partida, 2) == 0);
    VERIFICAR(resetearMazo(&partida) == 0);
    VERIFICAR(repartirManos(&partida) == 0);
    Contexto contextos[2] = {{0}};
    EstrategiaMus estrategias[NUMERO_JUGADORES_MUS] = {{0}};
    for (int jugador = 0; jugador < 2; jugador++) {
        estrategias[jugador].decidirMus = darMusUnaVez;
        estrategias[jugador].elegirDescartes = descartarPrimera;
        estrategias[jugador].decidirEnvite = pasar;
        estrategias[jugador].contexto = &contextos[jugador];
    }
    VERIFICAR(jugarFaseMus(&partida, estrategias) == 0);
    VERIFICAR(partida.baraja.siguiente_carta == 10);
    VERIFICAR(partida.descartes.siguiente_carta == 2);
    VERIFICAR(contextos[0].descartes == 1);
    VERIFICAR(contextos[1].descartes == 1);
    VERIFICAR(destruirPartidaMus(&partida) == 0);

    VERIFICAR(iniciarPartidaMusConJugadores(&partida, 2) == 0);
    VERIFICAR(resetearMazo(&partida) == 0);
    VERIFICAR(simularRondaMus(&partida) == 0);
    VERIFICAR(partida.baraja.siguiente_carta == 8);
    VERIFICAR(partida.mano == 1);
    VERIFICAR(resetearMazo(&partida) == 0);
    VERIFICAR(simularRondaMus(&partida) == 0);
    VERIFICAR(partida.mano == 0);
    VERIFICAR(destruirPartidaMus(&partida) == 0);

    VERIFICAR(simularPartidaMusConJugadores(3) == 1);
    VERIFICAR(simularPartidaMusConJugadores(2) == 0);
}

static void testLogDosJugadores(void) {
    PartidaMus partida = {0};
    VERIFICAR(iniciarPartidaMusConJugadores(&partida, 2) == 0);
    VERIFICAR(resetearMazo(&partida) == 0);
    VERIFICAR(repartirManos(&partida) == 0);
    partida.tantos[0] = 40;
    FILE *archivo = tmpfile();
    VERIFICAR(archivo != NULL);
    if (archivo != NULL) {
        fijarSalidaLog(archivo);
        fijarNivelLog(LOG_LANCES);
        VERIFICAR(logManos(LOG_LANCES, &partida) == 0);
        VERIFICAR(logGanadorLancePartida(LOG_LANCES, "Grande", &partida, 0) ==
                  0);
        VERIFICAR(logTantos(LOG_RONDAS, &partida) == 0);
        VERIFICAR(logGanadorPartida(LOG_RESULTADO, &partida) == 0);
        fijarNivelLog(LOG_SILENCIO);
        fijarSalidaLog(NULL);

        char contenido[1024] = {0};
        rewind(archivo);
        fread(contenido, 1, sizeof(contenido) - 1, archivo);
        fclose(archivo);
        VERIFICAR(strstr(contenido, "Jugador 0") != NULL);
        VERIFICAR(strstr(contenido, "Jugador 1") != NULL);
        VERIFICAR(strstr(contenido, "Jugador 2") == NULL);
        VERIFICAR(strstr(contenido, "Gana el jugador 0") != NULL);
        VERIFICAR(strstr(contenido, "pareja") == NULL);
    }
    VERIFICAR(destruirPartidaMus(&partida) == 0);
}

static void testProbabilidadDosJugadores(void) {
    Mano maxima = manoDe(REY, REY, REY, REY);
    VERIFICAR(probabilidadesVictoria1Fija(maxima, 0, GRANDE, NADA) == 1.0);
    double cedeEmpates =
        probabilidadesVictoria1Fija(maxima, 1, GRANDE, NADA);
    VERIFICAR(cedeEmpates > 0.0 && cedeEmpates < 1.0);
    VERIFICAR(probabilidadesVictoria1Fija(maxima, 0, PARES, NADA) == 1.0);
    VERIFICAR(probabilidadesVictoria1Fija(maxima, 2, GRANDE, NADA) == -1.0);
    destruirMano(&maxima);

    Mano intermedia = manoDe(REY, CABALLO, SEIS, AS);
    double grande =
        probabilidadesVictoria1Fija(intermedia, 0, GRANDE, NADA);
    VERIFICAR(grande > 0.0 && grande < 1.0);
    double juegoCondicionado = probabilidadesVictoria1Fija(
        intermedia, 0, JUEGO, TIENE_JUEGO);
    VERIFICAR(juegoCondicionado >= 0.0 && juegoCondicionado <= 1.0);
    destruirMano(&intermedia);
}

int main(void) {
    srand(2026);
    testInicializacionYReparto();
    testGanadoresYDesempate();
    testTanteoIndividual();
    testEnviteDosJugadores();
    testMusSimulacionYRotacion();
    testLogDosJugadores();
    testProbabilidadDosJugadores();
    return resumenPruebas("test_dos_jugadores");
}
