#include "Portas.h"

#define PINO_SENSOR_IR 8        // Pino do sensor IR de presença
#define PINO_ACTUADOR_PORTA 9   // Pino físico do LED/Motor da Porta

enum EstadoPorta { PORTA_FECHADA, PORTA_ABRINDO, PORTA_ABERTA, PORTA_FECHANDO };
static EstadoPorta estadoAtualPorta = PORTA_FECHADA;
static unsigned long tempoEstadoPorta = 0;

void inicializarPortas() {
  pinMode(PINO_SENSOR_IR, INPUT_PULLUP);
  pinMode(PINO_ACTUADOR_PORTA, OUTPUT);
  digitalWrite(PINO_ACTUADOR_PORTA, LOW);
  estadoAtualPorta = PORTA_FECHADA;
}

void comandarAberturaPorta() {
  if (estadoAtualPorta == PORTA_FECHADA || estadoAtualPorta == PORTA_FECHANDO) {
    estadoAtualPorta = PORTA_ABRINDO;
    tempoEstadoPorta = millis();
  }
}

bool lerSensorIR() {
  return (digitalRead(PINO_SENSOR_IR) == LOW); // Ajuste LOW/HIGH conforme seu sensor
}

void gerenciarMaquinaPortas() {
  unsigned long agora = millis();

  switch (estadoAtualPorta) {
    case PORTA_FECHADA:
      digitalWrite(PINO_ACTUADOR_PORTA, LOW);
      break;

    case PORTA_ABRINDO:
      digitalWrite(PINO_ACTUADOR_PORTA, HIGH); // Liga LED/Atuador indicando movimento
      if (agora - tempoEstadoPorta >= 1500) {  // 1.5s para abrir
        estadoAtualPorta = PORTA_ABERTA;
        tempoEstadoPorta = agora;
      }
      break;

    case PORTA_ABERTA:
      digitalWrite(PINO_ACTUADOR_PORTA, HIGH);
      // Se houver presença no feixe IR, renova o tempo para manter a porta aberta
      if (lerSensorIR()) {
        tempoEstadoPorta = agora;
      }
      if (agora - tempoEstadoPorta >= 3000) {  // Fica aberta por 3s
        estadoAtualPorta = PORTA_FECHANDO;
        tempoEstadoPorta = agora;
      }
      break;

    case PORTA_FECHANDO:
      digitalWrite(PINO_ACTUADOR_PORTA, HIGH);
      // Proteção: Obstáculo detectado enquanto fecha -> Reabre imediatamente
      if (lerSensorIR()) {
        estadoAtualPorta = PORTA_ABRINDO;
        tempoEstadoPorta = agora;
        break;
      }
      if (agora - tempoEstadoPorta >= 1500) {  // 1.5s para fechar
        estadoAtualPorta = PORTA_FECHADA;
        digitalWrite(PINO_ACTUADOR_PORTA, LOW);
      }
      break;
  }
}

bool portaEstaTotalmenteFechada() {
  return (estadoAtualPorta == PORTA_FECHADA);
}

bool portaEstaAberta() {
  return (estadoAtualPorta != PORTA_FECHADA);
}