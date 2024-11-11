#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 


#include "mus-sim.h"

int piedras[2];



void barajar(int baraja[], int tamano){
     // Inicializar el generador de números aleatorios
    for (int i = tamano - 1; i > 0; i--) {
        // Elegir un índice aleatorio entre 0 e i
        int j = rand() % (i + 1);

        // Intercambiar array[i] con array[j]
        int temp = baraja[i];
        baraja[i] = baraja[j];
        baraja[j] = temp;
    }
}

int ganadorGrande(Mano manos[]){
    int grande;
    int dentro[4] = {1, 1, 1, 1};
    for(int i = 0; i<4; i++){
        grande = pito;
        for(int j = 0; j<4; j++){
            if(dentro[j]){
                int carta_jugador = manos[j].cartas[i];
                if(carta_jugador == grande){

                } else if(carta_jugador > grande){
                    grande = carta_jugador;
                    for(int k = j-1; k>=0; k--)
                        dentro[k] = 0;
                } else
                    dentro[j] = 0;
            }
        }
    }
    for(int i = 0; i<4; i++){
        if(dentro[i])
            return i;
    }
}

int ganadorChica(Mano manos[4]){
    int chica;
    int dentro[4] = {1, 1, 1, 1};
    for(int i = 0; i<4; i++){
        chica = cerdo;
        for(int j = 0; j<4; j++){
            if(dentro[j]){
                int carta_jugador = manos[j].cartas[i];
                if(carta_jugador == chica){

                } else if(carta_jugador < chica){
                    chica = carta_jugador;
                    for(int k = j-1; j>=0; j--)
                        dentro[k] = 0;
                }
            }
        }
    }
    for(int i = 0; i<4; i++){
        if(dentro[i])
            return i;
    }
    return 0;
}

int ganadorPar( Mano manos[4]){
    int dentro[4] = {0, 0, 0, 0};
    int mayor = -1;
    int ganador;
    // Duplex
    for(int i = 0; i<4; i++)
        dentro[i] = (comprobarDuplex(manos[i]) != -1);
    // Arreglar
    for(int i = 0; i<4; i++)
        if(dentro[i]) return i;
    for(int i = 0; i<4; i++)
        dentro[i] = (comprobarMedias(manos[i]) != -1);
    for(int i = 0; i<4; i++)
        if(dentro[i]){
            if(comprobarMedias(manos[i])>mayor){
                mayor = comprobarMedias(manos[i]);
                ganador = i;
            }
        }
    if(mayor != -1) return ganador;
    for(int i = 0; i<4; i++)
        dentro[i] = (comprobarPar(manos[i]) != -1);
    for(int i = 0; i<4; i++)
        if(dentro[i]){
            if(comprobarPar(manos[i])>mayor){
                mayor = comprobarPar(manos[i]);
                ganador = i;
            }
        }
    if(mayor != -1) return ganador;
    return -1;
}

int comprobarPar( Mano mano){
    for(int i = 0; i<4; i++){
        for(int j = i+1; j<4; j++)
            if(mano.cartas[i] == mano.cartas[j])
                return mano.cartas[i];
    }
    return -1;
}

int comprobarMedias( Mano mano){
    int i;
    if((mano.cartas[0] == mano.cartas[1]) && (mano.cartas[0] == mano.cartas[2]))
        return mano.cartas[0];
    if((mano.cartas[0] == mano.cartas[1]) && (mano.cartas[0] == mano.cartas[3]))
        return mano.cartas[0];
    if((mano.cartas[0] == mano.cartas[2]) && (mano.cartas[0] == mano.cartas[3]))
        return mano.cartas[0];
    if((mano.cartas[1] == mano.cartas[2]) && (mano.cartas[1] == mano.cartas[3]))
        return mano.cartas[1];
    return -1;
}

int comprobarDuplex( Mano mano){
    int i;
    if((mano.cartas[0] == mano.cartas[1]) && (mano.cartas[2] == mano.cartas[3]))
        return 1;
    if((mano.cartas[0] == mano.cartas[2]) && (mano.cartas[3] == mano.cartas[1]))
        return 1;
    if((mano.cartas[0] == mano.cartas[3]) && (mano.cartas[1] == mano.cartas[2]))
        return 1;
    return -1;
}

int ganadorPunto( Mano manos[4]){
    int mayor = 0;;
    int ganador;
    int i;
    for(i = 0; i<4; i++){
        int suma_jugador = sumaMano(manos[i]);
        if(suma_jugador==31){
            mayor = suma_jugador;
            return i;
        }

    }
    for(i = 0; i<4; i++){
        int suma_jugador = sumaMano(manos[i]);
        if(suma_jugador==32){
            mayor = suma_jugador;
            return i;
        }

    }
    for(i = 0; i<4; i++){
        int suma_jugador = sumaMano(manos[i]);
        if(suma_jugador>mayor){
            mayor = suma_jugador;
            ganador = i;
        }

    }
    return ganador;
    
}

int sumaMano( Mano mano) {
    int i;
    int cuenta = 0;

    // Loop through each card in the hand
    for (i = 0; i < 4; i++) {
        int carta = mano.cartas[i];

        // Switch statement to add values based on the card
        switch (carta) {
            case pito:
                cuenta++;
                break;
            case cuatro:
                cuenta += 4;
                break;
            case cinco:
                cuenta += 5;
                break;
            case seis:
                cuenta += 6;
                break;
            case siete:
                cuenta += 7;
                break;
            default:
                cuenta += 10;  // Default case for face cards or other cards
        }
    }

    return cuenta;  // Return the calculated sum
}

int sacarCarta(int baraja[], int* tamano){
    int indice_aleatorio = rand() % *tamano;
    int carta_aleatoria = baraja[indice_aleatorio];
    quitarCarta(baraja, tamano, indice_aleatorio);
    return carta_aleatoria;

}

void quitarCarta(int baraja[], int* tamano, int indice){
    int i;
    for(i = indice; i<(*tamano-1); i++){
        baraja[i] = baraja[i+1];
    }
    int test = *tamano;
    (*tamano)= (*tamano)-1;
}

int darCarta(int baraja[], int* tamano, int figura){
    int i;
    for(i = 0; i<*tamano; i++){
        if(baraja[i] == figura){
            quitarCarta(baraja, tamano, i);
            break;
        }
            

    }
        
    return figura;
}

void crearBaraja(int baraja[]){
    int i = 0;
    for(; i<4; i++)
        baraja[i]=cuatro;
    for(; i<8; i++)
        baraja[i]=cinco;
    for(; i<12; i++)
        baraja[i]=seis;
    for(; i<16; i++)
        baraja[i]=siete;
    for(; i<20; i++)
        baraja[i]=sota;
    for(; i<24; i++)
        baraja[i]=caballo;
    for(; i<32; i++)
        baraja[i]=pito;
    for(; i<40; i++)
        baraja[i]=cerdo;
}

void ordenarManos( Mano manos[4]){
    for(int i = 0; i<4; i++){
        ordenarMano(manos[i].cartas);
    }
}

void ordenarMano(int cartas[]){
    for (int i = 0; i < 4 - 1; i++) {
        for (int j = 0; j < 4 - i - 1; j++) {
            if (cartas[j] > cartas[j + 1]) {
                // Intercambiar
                int temp = cartas[j];
                cartas[j] = cartas[j + 1];
                cartas[j + 1] = temp;
            }
        }
    }
}

