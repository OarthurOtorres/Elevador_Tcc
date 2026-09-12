#include "Logica.h"
#include <Arduino.h>
#include <Wire.h>

#define PCF_ADDR 0x27

#define S1 5
#define S2 6
#define S3 7

bool chamada[4] = {false, false, false, false};

int andarAtual = 0;
int andarDestino = 0;
int estado = 0;
int direcaoAtual = 0;

// Filtro de leitura: faz 5 amostragens no pino para evitar falsos negativos na partida
bool lerSensorComFiltro(int pino) {
  int contagemLow = 0;
  for (int i = 0; i < 5; i++) {
    if (digitalRead(pino) == LOW) {
      contagemLow++;
    }
    delay(10); // Intervalo para estabilização de leitura
  }
  return (contagemLow >= 4); // Considera ativo se pelo menos 4 leituras forem LOW
}

bool sensorAtivo(int andar) {
  if (andar == 1) return digitalRead(S1) == LOW;
  if (andar == 2) return digitalRead(S2) == LOW;
  if (andar == 3) return digitalRead(S3) == LOW;
  return false;
}

void detectarAndarInicial() {
  // Tempo para alimentação elétrica dos sensores estabilizar totalmente
  delay(200);

  if (lerSensorComFiltro(S1)) {
    andarAtual = 1;
    andarDestino = 1;
  } else if (lerSensorComFiltro(S2)) {
    andarAtual = 2;
    andarDestino = 2;
  } else if (lerSensorComFiltro(S3)) {
    andarAtual = 3;
    andarDestino = 3;
  } else {
    // Caso esteja parado entre dois andares
    andarAtual = 1;
    andarDestino = 1;
  }
}

void initBtsESensores() {
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);

  // Executa a leitura filtrada do andar no momento da energização
  detectarAndarInicial();

  Wire.begin();

  // Inicializa o PCF8574
  Wire.beginTransmission(PCF_ADDR);
  Wire.write(0b01101101);
  Wire.endTransmission();
}

void atualizarLedsBotoes() {
  // Cancela a chamada ao atingir o sensor do andar
  for (int i = 1; i <= 3; i++) {
    if (sensorAtivo(i)) {
      chamada[i] = false;
    }
  }

  uint8_t bytePCF = 0b01101101; 

  if (chamada[1]) bytePCF |= (1 << 1); // LED 1
  if (chamada[2]) bytePCF |= (1 << 4); // LED 2
  if (chamada[3]) bytePCF |= (1 << 7); // LED 3

  Wire.beginTransmission(PCF_ADDR);
  Wire.write(bytePCF);
  Wire.endTransmission();
}

void lerBotoes() {
  Wire.requestFrom((uint8_t)PCF_ADDR, (uint8_t)1);
  if (!Wire.available()) return;

  uint8_t leitura = Wire.read();

  bool bt1Pressionado = !(leitura & (1 << 0));
  bool bt2Pressionado = !(leitura & (1 << 3));
  bool bt3Pressionado = !(leitura & (1 << 6));

  if (bt1Pressionado && !sensorAtivo(1)) chamada[1] = true;
  if (bt2Pressionado && !sensorAtivo(2)) chamada[2] = true;
  if (bt3Pressionado && !sensorAtivo(3)) chamada[3] = true;

  atualizarLedsBotoes();
}

int escolherProximoAndar() {
  if (direcaoAtual == 1) {
    for (int i = andarAtual + 1; i <= 3; i++) {
      if (chamada[i]) return i;
    }
    direcaoAtual = -1;
  }

  if (direcaoAtual == -1) {
    for (int i = andarAtual - 1; i >= 1; i--) {
      if (chamada[i]) return i;
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

  return 0;
}