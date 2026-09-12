#include "Portas.h"
#include "DingDong.h"
#include <Arduino.h>
#include <Servo.h>

// Pino do sensor IR no D8 (INPUT_PULLUP)
#define SENSOR_IV_PIN 8 

// Pinos dos Servos das Portas
#define SERVO_P1 11
#define SERVO_P2 12
#define SERVO_P3 13

#define ANGULO_FECHADO 0
#define ANGULO_ABERTO  90

enum EstadoPorta {
  PORTA_FECHADA,
  PORTA_ABRINDO,
  PORTA_ABERTA,
  PORTA_FECHANDO
};

static EstadoPorta estadoAtualPorta = PORTA_FECHADA;
static int andarAtualPorta = 1;
static unsigned long tempoInicioEstado = 0;
static unsigned long tempoInicioObstrucao = 0;

Servo servoP1;
Servo servoP2;
Servo servoP3;

Servo* getServoAndar(int andar) {
  if (andar == 1) return &servoP1;
  if (andar == 2) return &servoP2;
  if (andar == 3) return &servoP3;
  return &servoP1;
}

// Com INPUT_PULLUP: Sem obstáculo = HIGH | Com obstáculo (bloqueado) = LOW
bool sensorObstaculoAtivo() {
  return (digitalRead(SENSOR_IV_PIN) == LOW);
}

void inicializarPortas() {
  pinMode(SENSOR_IV_PIN, INPUT_PULLUP); // Pino 8 configurado com pull-up interno

  servoP1.attach(SERVO_P1);
  servoP2.attach(SERVO_P2);
  servoP3.attach(SERVO_P3);

  servoP1.write(ANGULO_FECHADO);
  servoP2.write(ANGULO_FECHADO);
  servoP3.write(ANGULO_FECHADO);

  estadoAtualPorta = PORTA_FECHADA;
}

void comandarAberturaPorta(int andar) {
  andarAtualPorta = andar;
  estadoAtualPorta = PORTA_ABRINDO;
  tempoInicioEstado = millis();
  tempoInicioObstrucao = 0;

  Servo* s = getServoAndar(andarAtualPorta);
  if (!s->attached()) {
    s->attach(andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
  }
  s->write(ANGULO_ABERTO);
}

void gerenciarMaquinaPortas() {
  unsigned long agora = millis();
  bool haObstaculo = sensorObstaculoAtivo();

  switch (estadoAtualPorta) {
    
    case PORTA_FECHADA:
      setAlertaObstrucao(false);
      tempoInicioObstrucao = 0;
      break;

    case PORTA_ABRINDO:
      if (agora - tempoInicioEstado >= 1000) { // 1s para abrir
        estadoAtualPorta = PORTA_ABERTA;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = 0;
      }
      break;

    case PORTA_ABERTA:
      if (haObstaculo) {
        // Zera o cronômetro da porta: não fecha enquanto houver pessoa na passagem
        tempoInicioEstado = agora; 

        if (tempoInicioObstrucao == 0) {
          tempoInicioObstrucao = agora;
        }

        // Se a passagem continuar bloqueada por 4 segundos, dispara a sirene
        if (agora - tempoInicioObstrucao >= 4000) {
          setAlertaObstrucao(true);
        }
      } else {
        // Passagem livre: reseta o alarme
        tempoInicioObstrucao = 0;
        setAlertaObstrucao(false);

        // Aguarda 3 segundos de passagem livre para começar a fechar
        if (agora - tempoInicioEstado >= 3000) {
          estadoAtualPorta = PORTA_FECHANDO;
          tempoInicioEstado = agora;

          Servo* s = getServoAndar(andarAtualPorta);
          s->write(ANGULO_FECHADO);
        }
      }
      break;

    case PORTA_FECHANDO:
      // REABERTURA ANTI-ESMAGAMENTO
      if (haObstaculo) {
        estadoAtualPorta = PORTA_ABRINDO;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = agora;
        setAlertaObstrucao(false);

        Servo* s = getServoAndar(andarAtualPorta);
        s->write(ANGULO_ABERTO);
        break;
      }

      if (agora - tempoInicioEstado >= 1000) { // 1s para concluir fechamento
        estadoAtualPorta = PORTA_FECHADA;
        setAlertaObstrucao(false);
        tempoInicioObstrucao = 0;
      }
      break;
  }
}

bool portaEstaTotalmenteFechada() {
  return (estadoAtualPorta == PORTA_FECHADA);
}

void ativarPortaEmergencia(int andar) {
  setAlertaObstrucao(false);
  Servo* s = getServoAndar(andar);
  s->write(ANGULO_ABERTO);
  delay(500);
  s->detach();
}

void restaurarPortasAposEmergencia(int andar) {
  Servo* s = getServoAndar(andar);
  s->attach(andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
  s->write(ANGULO_FECHADO);
  estadoAtualPorta = PORTA_FECHADA;
}