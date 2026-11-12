#ifndef LOGICA_H
#define LOGICA_H

#include <Arduino.h>

extern bool chamada[4];
extern int andarAtual;
extern int andarDestino;
extern int estado;
extern int direcaoAtual;
extern unsigned int totalViagensAcumuladas;
extern bool modoManual;
extern bool referenciandoAtivo;
extern int estadoAnteriorLog;

void initBtsESensores();
void lerBotoes();
void atualizarLedsBotoes();
bool sensorAtivo(int andar);
int escolherProximoAndar();
void registrarViagemConcluida();
void executarHoming();
void iniciarReferenciamento();
void gerenciarReferenciamento();
void detectarAndarInicial();

#endif