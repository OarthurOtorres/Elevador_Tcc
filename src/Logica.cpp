#include "Logica.h"
#include <Arduino.h>

// Botões de chamada (INPUT_PULLUP)
#define Bt1 4
#define Bt2 5
#define Bt3 6
// Sensores de posição (INPUT_PULLUP)
#define S1 7
#define S2 8
#define S3 9

bool chamada[4] = {false, false, false, false};
int andarAtual = 1;
int andarDestino = 1;
int estado = 0;
int direcaoAtual = 0;

void initBtsESensores() {
  pinMode(Bt1, INPUT_PULLUP);
  pinMode(Bt2, INPUT_PULLUP);
  pinMode(Bt3, INPUT_PULLUP);
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
}

void lerBotoes() {
  if (!digitalRead(Bt1))
    chamada[1] = true;
  if (!digitalRead(Bt2))
    chamada[2] = true;
  if (!digitalRead(Bt3))
    chamada[3] = true;
}

int escolherProximoAndar() {
  if (direcaoAtual == 1) {
    for (int i = andarAtual + 1; i <= 3; i++) {
      if (chamada[i])
        return i;
    }
    direcaoAtual = -1;
  }

  if (direcaoAtual == -1) {
    for (int i = andarAtual - 1; i >= 1; i--) {
      if (chamada[i])
        return i;
    }
    direcaoAtual = 0;
  }

  if (direcaoAtual == 0) {
    for (int i = andarAtual + 1; i <= 3; i++) {
      if (chamada[i]) {
        direcaoAtual = 1;
        return i;
      }
    }
    for (int i = andarAtual - 1; i >= 1; i--) {
      if (chamada[i]) {
        direcaoAtual = -1;
        return i;
      }
    }
  }

  return andarAtual;
}

bool sensorAtivo(int andar) {
  if (andar == 1)
    return digitalRead(S1) == LOW;
  if (andar == 2)
    return digitalRead(S2) == LOW;
  if (andar == 3)
    return digitalRead(S3) == LOW;
  return false;
}