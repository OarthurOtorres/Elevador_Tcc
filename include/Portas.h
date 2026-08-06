#ifndef Portas_h
#define Portas_h

#include <Servo.h>

// Configurações mecânicas e de tempo
const int ANGULO_ABERTO  = 90; 
const int ANGULO_FECHADO = 0;
const unsigned long TEMPO_PORTA_ABERTA = 3000; // Tempo em ms que a porta fica aberta

// Declaração dos objetos Servo
extern Servo PortaAndar1;
extern Servo PortaAndar2;
extern Servo PortaAndar3;

// Funções públicas
void inicializarPortas();
void comandarAberturaPorta(int andar);
void gerenciarMaquinaPortas();
bool portaEstaTotalmenteFechada();

#endif