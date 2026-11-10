#include "Portas.h"
#include "DingDong.h"
#include <Arduino.h>
#include <Servo.h>

#define SENSOR_IV_PIN 8   // Pino do Sensor IR de Presença

#define SERVO_P1 A0       // Servo Porta 1º Andar
#define SERVO_P2 A1       // Servo Porta 2º Andar
#define SERVO_P3 A2       // Servo Porta 3º Andar

// CONFIGURAÇÃO DOS ÂNGULOS INDIVIDUAIS DE CADA PORTA
const int ANGULO_FECHADO_P1 = 5;
const int ANGULO_ABERTO_P1  = 95;

const int ANGULO_FECHADO_P2 = 5;  // Ajustado para 5, 95
const int ANGULO_ABERTO_P2  = 95;

const int ANGULO_FECHADO_P3 = 5;
const int ANGULO_ABERTO_P3  = 95;

const unsigned long TEMPO_MOVIMENTO_TOTAL = 1500; // Tempo de abertura/fechamento
const unsigned long TEMPO_PORTA_ABERTA     = 6000; // Tempo mantendo a porta aberta

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
static float anguloInicioMovimento = 0;

Servo servoP1;
Servo servoP2;
Servo servoP3;

Servo* getServoAndar(int andar) {
  if (andar == 1) return &servoP1;
  if (andar == 2) return &servoP2;
  if (andar == 3) return &servoP3;
  return &servoP1;
}

int getAnguloFechado(int andar) {
  if (andar == 1) return ANGULO_FECHADO_P1;
  if (andar == 2) return ANGULO_FECHADO_P2;
  if (andar == 3) return ANGULO_FECHADO_P3;
  return ANGULO_FECHADO_P1;
}

int getAnguloAberto(int andar) {
  if (andar == 1) return ANGULO_ABERTO_P1;
  if (andar == 2) return ANGULO_ABERTO_P2;
  if (andar == 3) return ANGULO_ABERTO_P3;
  return ANGULO_ABERTO_P1;
}

bool sensorObstaculoAtivo() {
  return (digitalRead(SENSOR_IV_PIN) == LOW);
}

bool lerSensorIR() {
  return sensorObstaculoAtivo();
}

void inicializarPortas() {
  pinMode(SENSOR_IV_PIN, INPUT_PULLUP);

  servoP1.attach(SERVO_P1, 500, 2500);
  servoP2.attach(SERVO_P2, 500, 2500);
  servoP3.attach(SERVO_P3, 500, 2500);

  servoP1.write(ANGULO_FECHADO_P1);
  servoP2.write(ANGULO_FECHADO_P2);
  servoP3.write(ANGULO_FECHADO_P3);

  estadoAtualPorta = PORTA_FECHADA;
}

void comandarAberturaPorta(int andar) {
  andarAtualPorta = andar;
  estadoAtualPorta = PORTA_ABRINDO;
  tempoInicioEstado = millis();
  tempoInicioObstrucao = 0;
  anguloInicioMovimento = getAnguloFechado(andar);

  Servo* s = getServoAndar(andarAtualPorta);
  if (!s->attached()) {
    int pino = (andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
    s->attach(pino, 500, 2500);
  }
}

void gerenciarMaquinaPortas() {
  unsigned long agora = millis();
  bool haObstaculo = sensorObstaculoAtivo();

  int angFechado = getAnguloFechado(andarAtualPorta);
  int angAberto  = getAnguloAberto(andarAtualPorta);
  Servo* s = getServoAndar(andarAtualPorta);

  switch (estadoAtualPorta) {
    
    case PORTA_FECHADA:
      setAlertaObstrucao(false);
      tempoInicioObstrucao = 0;
      break;

    case PORTA_ABRINDO: {
      float progresso = (float)(agora - tempoInicioEstado) / (float)TEMPO_MOVIMENTO_TOTAL;
      if (progresso >= 1.0f) {
        progresso = 1.0f;
        estadoAtualPorta = PORTA_ABERTA;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = 0;
      }
      
      int anguloCalculado = angFechado + (int)((angAberto - angFechado) * progresso);
      s->write(anguloCalculado);
      break;
    }

    case PORTA_ABERTA:
      if (haObstaculo) {
        tempoInicioEstado = agora; // Reinicia a contagem dos 6 segundos de porta aberta

        if (tempoInicioObstrucao == 0) {
          tempoInicioObstrucao = agora;
        }

        if (agora - tempoInicioObstrucao >= 4000) {
          setAlertaObstrucao(true);
        }
      } else {
        tempoInicioObstrucao = 0;
        setAlertaObstrucao(false);

        if (agora - tempoInicioEstado >= TEMPO_PORTA_ABERTA) {
          estadoAtualPorta = PORTA_FECHANDO;
          tempoInicioEstado = agora;
        }
      }
      break;

    case PORTA_FECHANDO: {
      if (haObstaculo) { // Reabertura suave imediata ao ler obstáculo
        estadoAtualPorta = PORTA_ABRINDO;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = agora;
        setAlertaObstrucao(false);
        break;
      }

      float progresso = (float)(agora - tempoInicioEstado) / (float)TEMPO_MOVIMENTO_TOTAL;
      if (progresso >= 1.0f) {
        progresso = 1.0f;
        estadoAtualPorta = PORTA_FECHADA;
        setAlertaObstrucao(false);
        tempoInicioObstrucao = 0;
      }

      int anguloCalculado = angAberto - (int)((angAberto - angFechado) * progresso);
      s->write(anguloCalculado);
      break;
    }
  }
}

bool portaEstaTotalmenteFechada() {
  return (estadoAtualPorta == PORTA_FECHADA);
}

bool portaEstaAberta() {
  return (estadoAtualPorta != PORTA_FECHADA);
}

void ativarPortaEmergencia(int andar) {
  setAlertaObstrucao(false);
  comandarAberturaPorta(andar); // Abre suavemente na emergência (1500 ms)
}

void restaurarPortasAposEmergencia(int andar) {
  andarAtualPorta = andar;
  estadoAtualPorta = PORTA_FECHANDO; // Fecha suavemente ao resetar (1500 ms)
  tempoInicioEstado = millis();
}