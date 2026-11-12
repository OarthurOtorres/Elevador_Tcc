#ifndef LOGICA_H
#define LOGICA_H

#include <Arduino.h>

extern bool chamada[4];
extern int andarAtual;
extern int andarDestino;
extern int estado;
extern int direcaoAtual;
extern unsigned int totalViagensAcumuladas;
extern bool modoManual; // Variável global do Modo Técnico/Admin

void initBtsESensores();
void lerBotoes();
void atualizarLedsBotoes();
bool sensorAtivo(int andar);
int escolherProximoAndar();
void registrarViagemConcluida();

#endif