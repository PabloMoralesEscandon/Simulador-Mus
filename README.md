# Simulador-Mus

Librería en C para aplicar las reglas del mus, simular partidas por parejas y
calcular probabilidades exactas de victoria por combinatoria, sin Monte Carlo.

## Funcionalidad

- Valoración y desempate por la mano en grande, chica, pares, juego y punto.
- Tanteo intrínseco de pares y juego, con partida a 40 tantos.
- Fase de mus con decisiones y descartes repetidos hasta que alguien corta.
- Envites, subidas, aceptación, rechazo y órdago en cada lance elegible.
- Estrategias inyectables e independientes para los cuatro jugadores.
- Simulación de manos y partidas completas con rotación de la mano.
- Probabilidad exacta de que la pareja 0-2 gane cualquiera de los cinco
  lances, con filtros opcionales sobre las manos rivales.

## Compilar y ejecutar

Requisitos: `gcc` y `make`. Las pruebas se compilan con ASan y UBSan.

```sh
make               # compila el simulador y ejecuta las pruebas
make test          # compila y ejecuta solo las pruebas
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
```

## Estrategias

Cada jugador aporta tres callbacks:

- `decidirMus`: devuelve 1 para dar mus o 0 para cortarlo.
- `elegirDescartes`: marca las cartas que cambia cuando todos dan mus.
- `decidirEnvite`: pasa, envida, quiere, no quiere o lanza un órdago.

Los callbacks reciben la mano, la posición del jugador, quién es mano, el
marcador y un contexto propio. `simularRondaMusConEstrategias` ejecuta una
mano sobre una partida existente; `simularPartidaMusConEstrategias` juega
hasta que una pareja alcanza 40 tantos.

## Probabilidades exactas

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
válidos.

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
- `tests/`: cinco suites unitarias.

## Modelo de reglas y límites

El motor usa baraja española de 40 cartas, ocho reyes y ocho ases, parejas
0-2 contra 1-3, desempate por la mano y partida a 40 tantos. Se tantean los
lances en orden: grande, chica, pares y juego o punto. Si solo una pareja es
elegible para pares o juego, no se abre envite en ese lance.

No se modelan señas, variantes regionales, vacas o torneos. Las estrategias
son callbacks: la librería aporta la estrategia pasiva de ejemplo, no una IA
competitiva. `experimentos/experimento2.c` conserva código histórico y no
forma parte del objetivo `make experimentos`.

## Pruebas y CI

`make test` compila y ejecuta las cinco suites con ASan y UBSan. GitHub
Actions las ejecuta en cada `push` y `pull request`.
