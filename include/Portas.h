#ifndef Portas_h
#define Portas_h

#include <Arduino.h>
#include <Servo.h>

const int ANGULO_ABERTO  = 90; 
const int ANGULO_FECHADO = 0;

const unsigned long TEMPO_PORTA_ABERTA = 3000; 

extern Servo servo1;
extern Servo servo2;
extern Servo servo3;

void inicializarPortas();
void comandarAberturaPorta(int andar);
void gerenciarMaquinaPortas();
bool portaEstaTotalmenteFechada();

// FUNÇÕES DE EMERGÊNCIA
void ativarPortaEmergencia(int andarAtual);
void restaurarPortasAposEmergencia(int andarAtual);

#endif