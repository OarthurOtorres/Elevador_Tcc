#ifndef PORTAS_H
#define PORTAS_H

#include <Arduino.h>

void inicializarPortas();
void comandarAberturaPorta(int andar);
void gerenciarMaquinaPortas();
bool portaEstaTotalmenteFechada();
bool portaEstaAberta();
bool lerSensorIR();
void ativarPortaEmergencia(int andar);
void restaurarPortasAposEmergencia(int andar);

#endif