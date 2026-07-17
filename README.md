# Simulador-Mus

Librería en C para simular partidas de mus y calcular probabilidades exactas de jugadas por combinatoria (sin Monte Carlo).

## Compilar y ejecutar

Requisitos: `gcc` y `make`. Todo se compila con sanitizers (ASan + UBSan).

```sh
make               # compila todo y corre los tests
make test          # solo los tests
make experimentos  # compila los experimentos en build/
make clean
```

Simular una partida (el argumento es el nivel de detalle, por defecto 3):

```sh
./partida_sim.out      # manos, lances, tantos y ganador
./partida_sim.out 2    # rondas y tantos
./partida_sim.out 1    # solo el resultado
```

Calcular una probabilidad desde código:

```c
Mano manos[2] = {/* manos fijas de los jugadores 0 y 2 */};
double p = probabilidadesVictoria2Fija(manos, 0, GRANDE, NADA, NADA);

## Componentes

| Módulo | Qué hace |
|---|---|
| `cartas` | Tipos genéricos (`Carta`, `Mano`, `Baraja`) y operaciones básicas: crear, destruir, barajar, robar. |
| `baraja_espanola` | Palos y números de la baraja española; creación de mazos de 40 y 48 cartas. |
| `mus` | Reglas del juego: valoración de manos, ganador de cada lance (grande, chica, pares, juego/punto) y gestión de partida (reparto, descartes, tantos). |
| `mus_sim` | Simulación de rondas y partidas completas a 40 tantos, y `probabilidadesVictoria2Fija`: probabilidad exacta de que una pareja gane un lance fijadas sus dos manos, enumerando todos los repartos rivales posibles con pesos combinatorios. |
| `mus_log` | Logger con niveles acumulativos (silencio → resultado → rondas → lances) y salida configurable (stdout o archivo). |

Los headers en `include/` llevan la documentación (formato doxygen). Además:

- `programas/partida_sim.c` — simula una partida y muestra su desarrollo.
- `experimentos/` — comparativas de estrategias (qué opción de juego es mejor en una situación dada).
- `tests/` — tests unitarios de todos los módulos.


```

## Tests y CI

`make test` compila y ejecuta las cinco suites (`tests/`). GitHub Actions las corre en cada push y pull request.

## Simplificaciones actuales

La simulación de partida no implementa envites: cada lance vale 1 tanto fijo, y pares y juego se puntúan siempre (no se comprueba quién puede ir a cada lance). Tampoco hay fase de mus/descartes durante la partida simulada: se juega a las cartas repartidas.

## Pendiente

- `probabilidadesVictoria2Fija` solo evalúa el lance de grande: falta despachar según el parámetro `ronda` (chica, pares, juego) y aplicar las restricciones `Condicion` a las manos rivales.
- Migrar `experimentos/experimento2.c` a la API actual (`experimento1` ya está migrado).
- Envites y apuestas en la simulación de partidas.
