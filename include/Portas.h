#ifndef PORTAS_H
#define PORTAS_H

void inicializarPortas();
void comandarAberturaPorta(int andar);
void gerenciarMaquinaPortas();
bool portaEstaTotalmenteFechada();
void ativarPortaEmergencia(int andar);
void restaurarPortasAposEmergencia(int andar);
bool sensorObstaculoAtivo(); // Retorna true se houver objeto no sensor IR

#endif