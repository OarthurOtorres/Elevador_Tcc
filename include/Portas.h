#ifndef PORTAS_H
#define PORTAS_H

#include <Arduino.h>

void inicializarPortas();
void comandarAberturaPorta();
void gerenciarMaquinaPortas();
bool portaEstaTotalmenteFechada();
bool portaEstaAberta();
bool lerSensorIR();

#endif