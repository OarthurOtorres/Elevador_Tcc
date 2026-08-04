#ifndef Logica_h
#define Logica_h

// Variáveis globais compartilhadas com o main.cpp
extern bool chamada[4];
extern int andarAtual;
extern int andarDestino;
extern int estado;
extern int direcaoAtual;

// Funções do sistema
void initBtsESensores();
void lerBotoes();
int escolherProximoAndar();
bool sensorAtivo(int andar);

#endif