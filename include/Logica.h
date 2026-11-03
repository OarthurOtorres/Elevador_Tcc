#ifndef LOGICA_H
#define LOGICA_H

#include <Arduino.h>

// Variáveis globais compartilhadas com todo o sistema
extern bool chamada[4];
extern int andarAtual;
extern int andarDestino;
extern int estado;
extern int direcaoAtual;
extern unsigned int totalViagensAcumuladas; // Contador global de viagens

// Funções do sistema
void initBtsESensores();
void lerBotoes();
void atualizarLedsBotoes(); // Atualiza o estado físico dos LEDs no PCF8574
int escolherProximoAndar();
bool sensorAtivo(int andar);
bool lerSensorComFiltro(int pino);
void registrarViagemConcluida(); // Função para incrementar e contabilizar a viagem

#endif