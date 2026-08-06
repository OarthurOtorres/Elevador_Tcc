#ifndef Portas_h
#define Portas_h

#include <Arduino.h>
#include <Servo.h>

const int ANGULO_ABERTO  = 90; 
const int ANGULO_FECHADO = 0;

// Tempo que a porta permanece aberta (em milissegundos)
const unsigned long TEMPO_PORTA_ABERTA = 3000; 

// Objetos Servo para cada andar
extern Servo servo1;
extern Servo servo2;
extern Servo servo3;

void inicializarPortas();
void comandarAberturaPorta(int andar);
void gerenciarMaquinaPortas();
bool portaEstaTotalmenteFechada();

#endif