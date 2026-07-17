#include "mus-sim.c"

void main(){
    int i = 0;
    int mano = 0;
    int postre = 0;
    int total = 0;
    int casos[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    srand(88);
    while(i<1000000){
        tamano_baraja = 40;
        crearBaraja(baraja);
        barajar(baraja, tamano_baraja);
         Mano jugador1;
        jugador1.cartas[0] = darCarta(baraja, &tamano_baraja, cerdo);
        jugador1.cartas[1] = darCarta(baraja, &tamano_baraja, cerdo);
        jugador1.cartas[2] = darCarta(baraja, &tamano_baraja, sota);
        jugador1.cartas[3] = darCarta(baraja, &tamano_baraja, seis);
         Mano jugador3;
        jugador3.cartas[0] = darCarta(baraja, &tamano_baraja, cerdo);
        jugador3.cartas[1] = darCarta(baraja, &tamano_baraja, siete);
        jugador3.cartas[2] = darCarta(baraja, &tamano_baraja, cuatro);
        jugador3.cartas[3] = darCarta(baraja, &tamano_baraja, pito);
        // Aleatorias
         Mano jugador2;
        jugador2.cartas[0] = sacarCarta(baraja, &tamano_baraja);
        jugador2.cartas[1] = sacarCarta(baraja, &tamano_baraja);
        jugador2.cartas[2] = sacarCarta(baraja, &tamano_baraja);
        jugador2.cartas[3] = sacarCarta(baraja, &tamano_baraja);

        for(int j = 0; j<4; j++){
            casos[jugador2.cartas[j]]++;
            total++;
        } 

         Mano jugador4;
        jugador4.cartas[0] = sacarCarta(baraja, &tamano_baraja);
        jugador4.cartas[1] = sacarCarta(baraja, &tamano_baraja);
        jugador4.cartas[2] = sacarCarta(baraja, &tamano_baraja);
        jugador4.cartas[3] = sacarCarta(baraja, &tamano_baraja);

        for(int j = 0; j<4; j++){
            casos[jugador4.cartas[j]]++;
            total++;
        }
        printf("Cartas 2: %d %d %d %d\n", jugador2.cartas[0], jugador2.cartas[1], jugador2.cartas[2], jugador2.cartas[3]);
        printf("Cartas 4: %d %d %d %d\n", jugador4.cartas[0], jugador4.cartas[1], jugador4.cartas[2], jugador4.cartas[3]);
        printf("####################################################\n");
        /*
        if(!((sumaMano(jugador2)>30) && (sumaMano(jugador4)>30)))
            continue;
        if((comprobarPar(jugador2)!=-1)&&(comprobarPar(jugador2)!=-1))
            continue;
        */
        Mano manos[4] = {jugador1, jugador2, jugador3, jugador4};
        ordenarManos(manos);
        // Experimento
        int noPareja = (comprobarPar(jugador2)== -1) && (comprobarPar(jugador4)== -1);
        int noMedias = (comprobarMedias(jugador2)== -1) && (comprobarMedias(jugador4)== -1);
        int noDuplex = (comprobarDuplex(jugador2)== -1) && (comprobarDuplex(jugador4)== -1);
        int ganaPunto = ganadorPunto(manos);
        if(noPareja){                                   // Si el rival no tiene pareja hay que ganar a punto
            if((ganaPunto == 0) || (ganaPunto == 2)){   // Se mira si se gana punto
                mano++;
            } else postre++;
        } else if(noMedias && noDuplex){                // Si tienen pareja se hace ordago a pareja
            mano++;                                     // Si no tienen medias ni duplex ganan
        } else postre++;                                // Si tienene pierden
        // Experimento final
        double porcentaje = postre > 0 ? ((double)mano/(mano+postre))*100 : 0;
        printf("Mano:%d    Postre:%d   Éxito:%.1f%%\n", mano, postre, porcentaje);
        for(int j=0; j<8; j++){
            printf("%.4f  ", (double)casos[j]/total);
        }
        printf("\n");
        printf("####################################################\n");
        i++;

    }
}