#include "Logica.h"
#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>

// Instância do PCF8574 no endereço I2C 0x27
PCF8574 pcf(0x27);

// Mapeamento dos pinos no PCF8574 para os botões e LEDs do elevador
#define PCF_BT1  0
#define PCF_LED1 1

#define PCF_BT2  3
#define PCF_LED2 4

#define PCF_BT3  6
#define PCF_LED3 7

// Sensores Hall de posição (Arduino)
#define S1 5
#define S2 6
#define S3 7

bool chamada[4] = {false, false, false, false};
int andarAtual = 1;
int andarDestino = 1;
int estado = 0;
int direcaoAtual = 0;

void initBtsESensores() {
  // Configuração dos Sensores Hall no Arduino
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);

  // Configuração dos Pinos no PCF8574
  pcf.pinMode(PCF_BT1, INPUT_PULLUP);
  pcf.pinMode(PCF_BT2, INPUT_PULLUP);
  pcf.pinMode(PCF_BT3, INPUT_PULLUP);

  pcf.pinMode(PCF_LED1, OUTPUT);
  pcf.pinMode(PCF_LED2, OUTPUT);
  pcf.pinMode(PCF_LED3, OUTPUT);

  // Inicializa os LEDs apagados (Nível HIGH desliga no modo Sink do PCF)
  pcf.digitalWrite(PCF_LED1, HIGH);
  pcf.digitalWrite(PCF_LED2, HIGH);
  pcf.digitalWrite(PCF_LED3, HIGH);

  pcf.begin();
}

void lerBotoes() {
  // Leitura com filtro Debounce via PCF8574
  if (pcf.digitalRead(PCF_BT1) == LOW) {
    delay(50);
    if (pcf.digitalRead(PCF_BT1) == LOW) {
      chamada[1] = true;
    }
  }

  if (pcf.digitalRead(PCF_BT2) == LOW) {
    delay(50);
    if (pcf.digitalRead(PCF_BT2) == LOW) {
      chamada[2] = true;
    }
  }

  if (pcf.digitalRead(PCF_BT3) == LOW) {
    delay(50);
    if (pcf.digitalRead(PCF_BT3) == LOW) {
      chamada[3] = true;
    }
  }

  // Atualiza as saídas dos LEDs imediatamente após a leitura
  atualizarLedsBotoes();
}

// Acende o LED quando a chamada é registrada e apaga quando o sensor Hall do andar é ativado
void atualizarLedsBotoes() {
  // Reseta a chamada assim que a cabine ativa o sensor Hall do andar correspondente
  if (sensorAtivo(1)) chamada[1] = false;
  if (sensorAtivo(2)) chamada[2] = false;
  if (sensorAtivo(3)) chamada[3] = false;

  // Atualiza os LEDs (LOW = Liga / HIGH = Desliga)
  pcf.digitalWrite(PCF_LED1, chamada[1] ? LOW : HIGH);
  pcf.digitalWrite(PCF_LED2, chamada[2] ? LOW : HIGH);
  pcf.digitalWrite(PCF_LED3, chamada[3] ? LOW : HIGH);
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

  return andarAtual;
}

bool sensorAtivo(int andar) {
  if (andar == 1) return digitalRead(S1) == LOW;
  if (andar == 2) return digitalRead(S2) == LOW;
  if (andar == 3) return digitalRead(S3) == LOW;
  return false;
}