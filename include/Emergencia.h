#ifndef Emergencia_h
#define Emergencia_h

// Variável para armazenar o estado do botão de emergência
extern volatile bool emergenciaAtivada;

// Funcoes relacionadas ao botão de emergência
void EmergenciaInit();
void ParadaEmergencia();
void rotinaSeguranca();
void ResetEmergencia();

#endif