#ifndef EMERGENCIA_H
#define EMERGENCIA_H

#include <Arduino.h>

extern volatile bool emergenciaAtivada;

void EmergenciaInit();
void ParadaEmergencia();
void ResetEmergencia();
bool tentarResetarSistema(); // Função de validação de reset
void rotinaSeguranca();

#endif