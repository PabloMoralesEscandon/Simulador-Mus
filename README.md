# Simulador-Mus

Librería en C para aplicar las reglas del mus, simular partidas de dos
jugadores o por parejas y calcular probabilidades exactas de victoria por
combinatoria, sin Monte Carlo.

## Funcionalidad

- Valoración y desempate por la mano en grande, chica, pares, juego y punto.
- Tanteo intrínseco de pares y juego, con partida a 40 tantos.
- Fase de mus con decisiones y descartes repetidos hasta que alguien corta.
- Envites, subidas, aceptación, rechazo y órdago en cada lance elegible.
- Estrategias inyectables e independientes para cada jugador activo.
- Simulación de manos y partidas completas con rotación de la mano.
- Modalidades de dos jugadores (uno contra uno) y cuatro jugadores (parejas
  0-2 contra 1-3).
- Probabilidad exacta de que la pareja 0-2 gane cualquiera de los cinco
  lances, con filtros opcionales sobre las manos rivales.
- Probabilidad exacta de victoria de una mano fija contra un rival en la
  modalidad de dos jugadores.

## Compilar y ejecutar

Requisitos: `gcc` y `make`. Las pruebas se compilan con ASan y UBSan. El
simulador y los experimentos se compilan con `-O2` y sin sanitizers.

```sh
make               # compila el simulador y ejecuta las pruebas
make test          # compila y ejecuta solo las pruebas
make test-release  # ejecuta las pruebas optimizadas sin sanitizers
make experimentos  # compila los experimentos mantenidos en build/
make clean
```

El programa incluido usa la estrategia pasiva predeterminada: corta el mus en
el primer turno y pasa en todos los envites. El motor admite estrategias
completas mediante `EstrategiaMus`.

```sh
./partida_sim.out      # manos, lances, tantos y ganador
./partida_sim.out 2    # rondas y tantos
./partida_sim.out 1    # solo el resultado
./partida_sim.out 0    # sin salida
./partida_sim.out 3 2  # detalle completo, partida de dos jugadores
```

El primer argumento es el nivel de log y el segundo es el número de jugadores
(`2` o `4`). La modalidad predeterminada sigue siendo la de cuatro jugadores.

Para crear una partida de dos jugadores desde la API:

```c
PartidaMus partida;
if (iniciarPartidaMusConJugadores(&partida, MUS_DOS_JUGADORES) != 0)
    return 1;
/* simularRondaMus reconstruye el mazo y juega una ronda completa. */
destruirPartidaMus(&partida);
```

`iniciarPartidaMus` conserva el comportamiento anterior e inicializa cuatro
jugadores. `PartidaMus.numeroJugadores` indica cuántas entradas de `manos` y
de los vectores de estrategias están activas.

## Estrategias

Cada jugador aporta tres callbacks:

- `decidirMus`: devuelve 1 para dar mus o 0 para cortarlo.
- `elegirDescartes`: marca las cartas que cambia cuando todos dan mus.
- `decidirEnvite`: pasa, envida, quiere, no quiere o lanza un órdago. Pasar
  ante un envite pendiente equivale a no quererlo.

Los callbacks reciben la mano, la posición del jugador, quién es mano, el
marcador y un contexto propio. `simularRondaMusConEstrategias` reconstruye el
mazo y ejecuta una mano sobre una partida existente. La fase de mus se aborta
con error tras `MAXIMO_RONDAS_FASE_MUS` rondas de descartes consecutivas. Para
una partida completa configurable se usa
`simularPartidaMusConEstrategiasYJugadores`; las funciones existentes sin
número de jugadores conservan la modalidad de cuatro. La partida termina
cuando uno de los dos equipos alcanza 40 tantos.

## Probabilidades exactas

En una partida de dos jugadores, `probabilidadesVictoria1Fija` fija la mano
del jugador 0 y enumera todas las manos físicas posibles del rival:

```c
double p = probabilidadesVictoria1Fija(
    miMano, 0, GRANDE, NADA);
```

`probabilidadesVictoria2Fija` fija las manos de los jugadores 0 y 2 y enumera
todos los repartos físicos posibles para 1 y 3. Cada composición se pondera
por el número de cartas reales que la produce.

```c
Mano manos[2] = {/* manos físicas válidas de los jugadores 0 y 2 */};
double p = probabilidadesVictoria2Fija(
    manos, 0, GRANDE, NADA, NADA);
```

El parámetro `ronda` acepta `GRANDE`, `CHICA`, `PARES`, `JUEGO` o `PUNTO`.
Los filtros rivales disponibles son `NADA`, `TIENE_JUEGO`, `TIENE_31` y
`PAR_Y_JUEGO`. En pares y juego, un reparto en el que ese lance no existe no
cuenta como victoria; punto solo se disputa si nadie tiene juego. La función
devuelve `-1.0` si las manos, la posición, el lance o los filtros no son
válidos, y también si los filtros dejan vacío el espacio muestral.

## Componentes

| Módulo | Responsabilidad |
|---|---|
| `cartas` | Tipos genéricos y operaciones sobre cartas, manos y barajas. |
| `baraja_espanola` | Palos, números y creación de barajas de 40 y 48 cartas. |
| `mus` | Valoración, ganadores, tanteo, descartes y estado de envites. |
| `mus_sim` | Fases de mus, negociación, rondas, partidas y probabilidades. |
| `mus_log` | Registro configurable desde silencio hasta detalle de lances. |

La API pública está documentada en `include/`. También se incluyen:

- `programas/partida_sim.c`: ejecutable de simulación.
- `experimentos/experimento1.c`: consulta de probabilidad exacta.
- `tests/`: seis suites unitarias.

## Modelo de reglas y límites

El motor usa baraja española de 40 cartas, ocho reyes y ocho ases, desempate
por la mano y partida a 40 tantos. Con dos jugadores, cada jugador es su propio
equipo y solo tantea el valor intrínseco de su mano; con cuatro, juegan las
parejas 0-2 contra 1-3 y se suman las jugadas de ambos compañeros. Se tantean
los lances en orden: grande, chica, pares y juego o punto. Si solo un equipo es
elegible para pares o juego, no se abre envite en ese lance.

No se modelan señas, variantes regionales, vacas o torneos. Las estrategias
son callbacks: la librería aporta la estrategia pasiva de ejemplo, no una IA
competitiva.

## Pruebas y CI

`make test` compila y ejecuta las seis suites con ASan y UBSan. GitHub Actions
las ejecuta con GCC y Clang, y repite la batería optimizada sin sanitizers, en
cada `push` y `pull request`.
