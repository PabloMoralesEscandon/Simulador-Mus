#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "mus_sim.h"

int main(void) {
    srand((unsigned)time(NULL) ^ ((unsigned)getpid() << 16));
    fijarVerbosidadSim(1);
    return simularPartidaMus();
}
