#include <stdlib.h>
#include <time.h>

#include "mus_sim.h"

int main(void) {
    srand(time(NULL));
    fijarVerbosidadSim(1);
    return simularPartidaMus();
}
