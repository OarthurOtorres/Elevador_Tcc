#ifndef PORTAS_H
#define PORTAS_H

#include <Arduino.h>

extern unsigned long tempoPortaAbertaMs;

void inicializarPortas();
void gerenciarMaquinaPortas();
void comandarAberturaPorta(int andar);
void comandarTogglePortaEspecifica(int andar);
bool portaEstaTotalmenteFechada();
bool portaEstaAberta();
void ativarPortaEmergencia(int andar);
void restaurarPortasAposEmergencia(int andar);
bool lerSensorIR();
void salvarTempoPorta(unsigned long tempoMs);

#endif