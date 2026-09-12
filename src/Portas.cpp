#include "Portas.h"
#include "DingDong.h"
#include <Arduino.h>
#include <Servo.h>

// ============================================================================
// MAPA DE PINOS REAIS DO SEU HARDWARE
// ============================================================================
#define SENSOR_IV_PIN 8   // Pino do Sensor IR

#define SERVO_P1 A0       // Servo Porta 1º Andar
#define SERVO_P2 A1       // Servo Porta 2º Andar
#define SERVO_P3 A2       // Servo Porta 3º Andar

#define ANGULO_FECHADO 0
#define ANGULO_ABERTO  90
// ============================================================================

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

bool sensorObstaculoAtivo() {
  return (digitalRead(SENSOR_IV_PIN) == LOW);
}

void inicializarPortas() {
  pinMode(SENSOR_IV_PIN, INPUT_PULLUP);

  // Anexa os servos nos pinos analógicos (A0, A1, A2 funcionam perfeitamente como PWM de servo)
  servoP1.attach(SERVO_P1, 500, 2500);
  servoP2.attach(SERVO_P2, 500, 2500);
  servoP3.attach(SERVO_P3, 500, 2500);

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
    int pino = (andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
    s->attach(pino, 500, 2500);
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
      getServoAndar(andarAtualPorta)->write(ANGULO_ABERTO);

      if (agora - tempoInicioEstado >= 1200) { 
        estadoAtualPorta = PORTA_ABERTA;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = 0;
      }
      break;

    case PORTA_ABERTA:
      if (haObstaculo) {
        tempoInicioEstado = agora; 

        if (tempoInicioObstrucao == 0) {
          tempoInicioObstrucao = agora;
        }

        if (agora - tempoInicioObstrucao >= 4000) {
          setAlertaObstrucao(true);
        }
      } else {
        tempoInicioObstrucao = 0;
        setAlertaObstrucao(false);

        if (agora - tempoInicioEstado >= 3000) {
          estadoAtualPorta = PORTA_FECHANDO;
          tempoInicioEstado = agora;

          Servo* s = getServoAndar(andarAtualPorta);
          s->write(ANGULO_FECHADO);
        }
      }
      break;

    case PORTA_FECHANDO:
      getServoAndar(andarAtualPorta)->write(ANGULO_FECHADO);

      if (haObstaculo) { // Reabertura de emergência se alguém passar na porta
        estadoAtualPorta = PORTA_ABRINDO;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = agora;
        setAlertaObstrucao(false);

        Servo* s = getServoAndar(andarAtualPorta);
        s->write(ANGULO_ABERTO);
        break;
      }

      if (agora - tempoInicioEstado >= 1200) { 
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
  int pino = (andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
  s->attach(pino, 500, 2500);
  s->write(ANGULO_FECHADO);
  estadoAtualPorta = PORTA_FECHADA;
}