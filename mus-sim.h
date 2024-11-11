enum figura  {pito, cuatro, cinco, seis, siete, sota, caballo, cerdo};

typedef struct { 
  int cartas[4];
} Mano;

int tamano_baraja = 40;
int baraja[40];

int tamano_descartes = 0;
int descartes[40];

void barajar(int baraja[], int tamano);

int ganadorGrande(Mano manos[4]);

int ganadorChica( Mano manos[4]);

int ganadorPar( Mano manos[4]);

int comprobarPar( Mano mano);

int comprobarMedias( Mano mano);

int comprobarDuplex( Mano mano);

int ganadorPunto( Mano manos[4]);

int sumaMano( Mano mano);

int sacarCarta(int baraja[], int* tamano);

void quitarCarta(int baraja[], int* tamano, int indice);

int darCarta(int baraja[], int* tamano, int figura);

void ordenarMano(int cartas[]);

void ordenarManos( Mano manos[4]);

void crearBaraja(int baraja[]);